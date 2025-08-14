// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyGameInstance.h"
#include "MyGameModeBase.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	//Subscribe to OnPhysicsUpdated in the Movement Component
	MovementComponent->OnPhysicsUpdated.AddDynamic(this, &APlayerPawn::RecordPlayerInfo);
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: figure out if there is a better place to run it
	RunCameraEffects(DeltaTime);
}

#pragma region Camera

void APlayerPawn::RunCameraEffects(float DeltaTime)
{
	// TODO: Motion blur

	LeanCamera();
	CameraShake();
	ChangeCameraFOV(DeltaTime);
}

void APlayerPawn::LeanCamera()
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

void APlayerPawn::SetLeanSettings(float Roll, float InterpSpeed)
{
	FRotator currentRotation = Camera->GetRelativeRotation();
	FRotator targetRotation = currentRotation;
	targetRotation.Roll = Roll;

	FRotator newRotation = UKismetMathLibrary::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	Camera->SetRelativeRotation(newRotation);
}

void APlayerPawn::CameraShake()
{
	if (MovementComponent->GetCurrentVelocity() > FastVelocityThreshold)
	{
		// Done because this is easier in blueprints
		CameraShakeBP();
	}
}

void APlayerPawn::ChangeCameraFOV(float DeltaTime)
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

void APlayerPawn::SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime)
{
	float currentFOV = Camera->FieldOfView;
	float newFOV = UKismetMathLibrary::FInterpTo(currentFOV, FOV, DeltaTime, InterpSpeed);
	Camera->SetFieldOfView(newFOV);
}

#pragma endregion

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseThrottle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseHandbrake);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseSteer);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivatePauseBP);

		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateReset);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateUseItem);

		SteeringAxisBinding = EnhancedInputComponent->BindActionValue(SteeringAction);
	}
}

void APlayerPawn::RecordPlayerInfo()
{
	int32 loopNum = GameMode->GetCurrentLoopNumber();
	
	if (loopNum > -1)
	{
		GameInstance->PlayerSpeed[loopNum].ArrayOfFloats.Emplace(MovementComponent->GetCurrentSpeed());
		GameInstance->PlayerSteering[loopNum].ArrayOfFloats.Emplace(MovementComponent->GetCurrentSteering());
		GameInstance->PlayerWantsToGoForwardOrBackwards[loopNum].ArrayOfBools.Emplace(MovementComponent->GetCurrentWantsToGoForwardOrBackwards());
		GameInstance->PlayerSteerDirections[loopNum].ArrayOfDirections.Emplace(MovementComponent->GetCurrentSteerDirection());

		GameInstance->PlayerTransforms[loopNum].ArrayOfTransforms.Emplace(MovementComponent->GetVehicle()->GetActorTransform());
	}
}

void APlayerPawn::OnActivateReset(const FInputActionValue& value)
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

void APlayerPawn::OnActivateUseItem(const FInputActionValue& value)
{
	InventoryComponent->UseItem(value.Get<float>());
}

void APlayerPawn::OnActivateThrottle(const FInputActionValue& value)
{
	MovementComponent->Throttle(value.Get<float>());
}

void APlayerPawn::OnActivateBrake(const FInputActionValue& value)
{
	MovementComponent->Brake();
}

void APlayerPawn::OnActivateHandbrake(const FInputActionValue& value)
{
	MovementComponent->Handbrake();
}

void APlayerPawn::OnActivateSteer(const FInputActionValue& value)
{
	MovementComponent->Steer(value.Get<float>());
}

void APlayerPawn::OnReleaseThrottle(const FInputActionValue& value)
{
	MovementComponent->ReleaseThrottle();
}

void APlayerPawn::OnReleaseBrake(const FInputActionValue& value)
{
	MovementComponent->ReleaseBrake();
}

void APlayerPawn::OnReleaseHandbrake(const FInputActionValue& value)
{
	MovementComponent->ReleaseHandbrake();
}

void APlayerPawn::OnReleaseSteer(const FInputActionValue& value)
{
	MovementComponent->ReleaseSteer();
}