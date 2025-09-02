// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncPlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyGameInstance.h"
#include "MyGameModeBase.h"

// Sets default values
AAsyncPlayerPawn::AAsyncPlayerPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Camera

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->SocketOffset.Z = 140.0f;

	OriginalFOV = Camera->FieldOfView;

#pragma endregion
}

// Called when the game starts or when spawned
void AAsyncPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	//Get reference to the game mode
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (MovementComponent)
	{
		//Subscribe to OnPhysicsUpdated in the Movement Component
		MovementComponent->OnAsyncPhysicsUpdated.AddDynamic(this, &AAsyncPlayerPawn::RecordPlayerInfo);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Player Pawn Subscribed to movement updates."));
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to subscribe to movement updates: Movement Component invalid!"));
	}
}

// Called every frame
void AAsyncPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: figure out if there is a better place to run it
	RunCameraEffects(DeltaTime);
}

#pragma region Camera

void AAsyncPlayerPawn::RunCameraEffects(float DeltaTime)
{
	// TODO: Motion blur

	LeanCamera();
	CameraShake();
	ChangeCameraFOV(DeltaTime);
}

void AAsyncPlayerPawn::LeanCamera()
{
	ESteerDirection currentSteerDirection = MovementComponent->GetCurrentSteerDirection();

	if (MovementComponent->GetCurrentVelocity() > FastVelocityThreshold)
	{
		// Workaround for input get value not working
		if (currentSteerDirection == ESteerDirection::RIGHT)
		{
			// Lean camera right
			SetLeanSettings(CameraLeanAmount, CameraInterpSpeed);
		}
		else if (currentSteerDirection == ESteerDirection::LEFT)
		{
			// Lean camera left
			SetLeanSettings(-1 * CameraLeanAmount, CameraInterpSpeed);
		}
	}

	if (currentSteerDirection == ESteerDirection::STRAIGHT)
	{
		// Stop lean camera
		SetLeanSettings(0, CameraInterpSpeed);
	}
}

void AAsyncPlayerPawn::SetLeanSettings(float Roll, float InterpSpeed)
{
	FRotator currentRotation = Camera->GetRelativeRotation();
	FRotator targetRotation = currentRotation;
	targetRotation.Roll = Roll;

	FRotator newRotation = UKismetMathLibrary::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	Camera->SetRelativeRotation(newRotation);
}

void AAsyncPlayerPawn::CameraShake()
{
	if (MovementComponent->GetCurrentVelocity() > FastVelocityThreshold)
	{
		// Done because this is easier in blueprints
		PlayerCameraShakeBP();
	}
}

void AAsyncPlayerPawn::ChangeCameraFOV(float DeltaTime)
{
	float speed = FMath::Abs(MovementComponent->GetCurrentVelocity());

	if (speed > FastVelocityThreshold)
	{
		float targetFOV = OriginalFOV * (1 + (SpeedFOVEffect * speed / 100000));
		SetFOVSettings(targetFOV, CameraInterpSpeed, DeltaTime);
	}
	else
	{
		SetFOVSettings(OriginalFOV, CameraInterpSpeed, DeltaTime);
	}
}

void AAsyncPlayerPawn::SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime)
{
	float currentFOV = Camera->FieldOfView;
	float newFOV = UKismetMathLibrary::FInterpTo(currentFOV, FOV, DeltaTime, InterpSpeed);
	Camera->SetFieldOfView(newFOV);
}

#pragma endregion

// Called to bind functionality to input
void AAsyncPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			InputSubsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* MyInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent = MyInputComponent;
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AAsyncPlayerPawn::OnActivateThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AAsyncPlayerPawn::OnReleaseThrottle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AAsyncPlayerPawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AAsyncPlayerPawn::OnReleaseBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AAsyncPlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AAsyncPlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AAsyncPlayerPawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AAsyncPlayerPawn::OnReleaseHandbrake);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Started, this, &AAsyncPlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &AAsyncPlayerPawn::OnReleaseSteer);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AAsyncPlayerPawn::OnActivatePauseBP);

		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &AAsyncPlayerPawn::OnActivateReset);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &AAsyncPlayerPawn::OnActivateUseItem);

		SteeringAxisBinding = EnhancedInputComponent->BindActionValue(SteeringAction);
	}
}

void AAsyncPlayerPawn::RecordPlayerInfo()
{
	/*
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Recording player info."));
	*/
	int32 loopNum = GameMode->GetCurrentLoopNumber();

	if (loopNum > -1)
	{
		GameInstance->PlayerSpeed[loopNum].ArrayOfFloats.Emplace(MovementComponent->GetCurrentSpeed());
		GameInstance->PlayerSteering[loopNum].ArrayOfFloats.Emplace(MovementComponent->GetCurrentSteering());
		GameInstance->PlayerWantsToGoForwardOrBackwards[loopNum].ArrayOfBools.Emplace(MovementComponent->GetCurrentWantsToGoForwardOrBackwards());
		GameInstance->PlayerSteerDirections[loopNum].ArrayOfDirections.Emplace(MovementComponent->GetCurrentSteerDirection());

		GameInstance->PlayerTransforms[loopNum].ArrayOfTransforms.Emplace(GetActorTransform());
	}
}

void AAsyncPlayerPawn::OnActivateReset(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();

	if (axisValue != 0)
	{
		if (GameMode)
		{
			GameMode->RestartThisLoop();
		}
	}
}

void AAsyncPlayerPawn::OnActivateUseItem(const FInputActionValue& value)
{
	InventoryComponent->UseItem(value.Get<float>());
}

void AAsyncPlayerPawn::OnActivateThrottle(const FInputActionValue& value)
{
	MovementComponent->Throttle(value.Get<float>());
}

void AAsyncPlayerPawn::OnActivateBrake(const FInputActionValue& value)
{
	MovementComponent->Brake();
}

void AAsyncPlayerPawn::OnActivateHandbrake(const FInputActionValue& value)
{
	MovementComponent->Handbrake();
}

void AAsyncPlayerPawn::OnActivateSteer(const FInputActionValue& value)
{
	MovementComponent->Steer(value.Get<float>());
}

void AAsyncPlayerPawn::OnReleaseThrottle(const FInputActionValue& value)
{
	MovementComponent->ReleaseThrottle();
}

void AAsyncPlayerPawn::OnReleaseBrake(const FInputActionValue& value)
{
	MovementComponent->ReleaseBrake();
}

void AAsyncPlayerPawn::OnReleaseHandbrake(const FInputActionValue& value)
{
	MovementComponent->ReleaseHandbrake();
}

void AAsyncPlayerPawn::OnReleaseSteer(const FInputActionValue& value)
{
	MovementComponent->ReleaseSteer();
}