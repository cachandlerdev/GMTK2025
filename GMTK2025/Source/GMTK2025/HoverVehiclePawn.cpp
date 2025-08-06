// Fill out your copyright notice in the Description page of Project Settings.


#include "HoverVehiclePawn.h"
//#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MyGameModeBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "Components/AudioComponent.h"


// Sets default values
AHoverVehiclePawn::AHoverVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(132.808112f, 79.510442, 86.426227));
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(BoxCollision);
	Chassis->SetSimulatePhysics(true);
	
	Chassis->BodyInstance.bOverrideMass = true;
	//Chassis->SetMassOverrideInKg("", 50000.0);
	Chassis->GetBodyInstance()->SetMassOverride(50000.0, true);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);

	// spheres
	FrontSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FrontSphereCollision"));
	FrontSphere->SetSphereRadius(90.0f);
	FrontSphere->SetupAttachment(BoxCollision);
	FrontSphere->SetGenerateOverlapEvents(false);
	FrontSphere->SetRelativeLocation(FVector(106.0f, 0.0f, 0.0f));
	
	BackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BackSphereCollision"));
	BackSphere->SetSphereRadius(90.0f);
	BackSphere->SetupAttachment(BoxCollision);
	BackSphere->SetGenerateOverlapEvents(false);
	BackSphere->SetRelativeLocation(FVector(-133.0f, 0.0f, 0.0f));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	
	RightThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightThruster"));
	RightThrusterParticleComponent->SetupAttachment(Chassis);
	RightThrusterParticleComponent->SetRelativeLocation(FVector(-278.666667,34.333334,85.666667));
	RightThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	RightThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));
	
	LeftThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftThruster"));
	LeftThrusterParticleComponent->SetupAttachment(Chassis);
	LeftThrusterParticleComponent->SetRelativeLocation(FVector(-278.666667,-27.666667,85.666667));
	LeftThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	LeftThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));
	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->SocketOffset.Z = 140.0f;

	OriginalFOV = Camera->FieldOfView;

	CarWindComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarWindComponent"));
	CarWindComponent->SetupAttachment(RootComponent);
	CarEngineLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarEngineLoopComponent"));
	CarEngineLoopComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHoverVehiclePawn::BeginPlay()
{
	Super::BeginPlay();

	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	GetWorldTimerManager().SetTimer(PhysicsUpdateHandle, this, &AHoverVehiclePawn::UpdateMovementPhysics,
		PhysicsUpdateTime, true);
}

// Called every frame
void AHoverVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunCameraEffects();
}

bool AHoverVehiclePawn::ShouldApplyMovement()
{
	
	//Check if the vehicle is EMP'd
	if (IsEMPd)
	{
		return false;
	}
	
	FHitResult HitResult;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= MaxDistanceToFloor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	return bHit;
}

void AHoverVehiclePawn::ApplyPlayerMovement()
{
	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsMovementFramerateCompensation);
	float RotationAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsRotationFramerateCompensation);
	if (bWantsToGoForwardOrBackwards)
	{
		FVector force = Chassis->GetForwardVector();
		//force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		//force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		
		// Thanks unreal for making physics framerate dependent
		force.X *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * SpeedMultiplier;
		force.Y *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * SpeedMultiplier;
		force.Z = HoverAmount;
		
		BoxCollision->AddForce(force, "", true);
	}

	if (MySteerDirection != ESteerDirection::STRAIGHT)
	{
		//FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * SteeringMultiplier);
		FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * RotationAccountForFramerate * SteeringMultiplier);
		BoxCollision->AddTorqueInDegrees(torque, "", true);
	}
}

void AHoverVehiclePawn::RecordPlayerInfo()
{
	int32 loopNum = GameMode->GetCurrentLoopNumber();
		
	if (loopNum > -1)
	{
		GameInstance->PlayerSpeed[loopNum].ArrayOfFloats.Emplace(Speed);
		GameInstance->PlayerSteering[loopNum].ArrayOfFloats.Emplace(Steering);
		GameInstance->PlayerWantsToGoForwardOrBackwards[loopNum].ArrayOfBools.Emplace(bWantsToGoForwardOrBackwards);
		GameInstance->PlayerSteerDirections[loopNum].ArrayOfDirections.Emplace(MySteerDirection);
		
		GameInstance->PlayerTransforms[loopNum].ArrayOfTransforms.Emplace(GetActorTransform());
	}
}

void AHoverVehiclePawn::UpdateMovementPhysics()
{
	if (ShouldApplyMovement())
	{
		ApplyPlayerMovement();
	}

	//Store player info to game instance for the ghost, every second
	RecordPlayerInfo();
}

void AHoverVehiclePawn::ApplyLongBoost()
{
	if (RemainingLongBoostTime <= 0)
	{
		GetWorldTimerManager().ClearTimer(LongBoostDurationHandle);
	}
	else
	{
		const float baseBoostMultiplier = 100000.0f;
		FVector direction = RootComponent->GetForwardVector();
		BoxCollision->AddForce(direction * LongBoostStrengthMultiplier * baseBoostMultiplier, "", true);
		
		RemainingLongBoostTime = RemainingLongBoostTime - LongBoostUpdateTime;
	}
}

// Called to bind functionality to input
void AHoverVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			InputSubsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* MyInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent = MyInputComponent;
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnReleaseThrottle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnReleaseBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AHoverVehiclePawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Started, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnReleaseSteer);
		
		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateReset);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateUseItem);

		SteeringAxisBinding = EnhancedInputComponent->BindActionValue(SteeringAction);
	}
}

void AHoverVehiclePawn::Boost(float BoostStrength)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EngineShortBoostSound, GetActorLocation());
	const float baseBoostMultiplier = 100000.0f;
	FVector direction = RootComponent->GetForwardVector();
	BoxCollision->AddForce(direction * BoostStrength * baseBoostMultiplier, "", true);
}

void AHoverVehiclePawn::LongBoost(float BoostStrength, float Duration)
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), EngineLongBoostSound, GetActorLocation());
	if (RemainingLongBoostTime <= 0)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EngineLongBoostSound, GetActorLocation());
		RemainingLongBoostTime = Duration;
		LongBoostStrengthMultiplier = BoostStrength;
		GetWorldTimerManager().SetTimer(LongBoostDurationHandle, this, &AHoverVehiclePawn::ApplyLongBoost,
			LongBoostUpdateTime, true);	
	}
}

void AHoverVehiclePawn::EMP(float Duration)
{
	if(!IsEMPd) 
	{
		IsEMPd = true;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SlowDownPadSound, GetActorLocation());

		GetWorldTimerManager().SetTimer(EMPDurationHandle, this, &AHoverVehiclePawn::EndEMP,
			Duration, false);
	}	
}

void AHoverVehiclePawn::EndEMP()
{
	IsEMPd = false;

	GetWorldTimerManager().ClearTimer(EMPDurationHandle);
}

void AHoverVehiclePawn::Inverter(float Duration)
{
	if (!IsInverted)
	{
		IsInverted = true;

		GetWorldTimerManager().SetTimer(InverterDurationHandle, this, &AHoverVehiclePawn::EndInverter,
			Duration, false);
	}
}

void AHoverVehiclePawn::EndInverter()
{
	IsInverted = false;

	GetWorldTimerManager().ClearTimer(InverterDurationHandle);
}

void AHoverVehiclePawn::StopMovement()
{
	Speed = 0;
	Steering = 0;
	bWantsToGoForwardOrBackwards = false;
	MySteerDirection = ESteerDirection::STRAIGHT;

	BoxCollision->SetSimulatePhysics(false);
	BoxCollision->SetSimulatePhysics(true);
}
float AHoverVehiclePawn::GetSpeed()
{
	return Speed;
}

float AHoverVehiclePawn::GetCurrentVelocityInKMPerHour()
{
	FVector VelocityVector = GetVelocity();

	double VelocityInCmPerSecond = VelocityVector.Length();

	double VelocityInKMPerHour = VelocityInCmPerSecond * 0.036;

	return VelocityInKMPerHour;
}

TArray<int> AHoverVehiclePawn::GetItems()
{
	TArray<int> Items;
	Items.Init(1, 2);

	return Items;

}

void AHoverVehiclePawn::AddVehicleItem(TSubclassOf<UVehicleItems> VehicleItemClass)
{
	if (VehicleItem != nullptr)
	{
		VehicleItem->RemoveItem();
	}
	if (GEngine)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("%s"), VehicleItemClass->GetName());
	}
	
	UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(this, VehicleItemClass);
	if (NewVehicleItem)
	{
		NewVehicleItem->RegisterComponent();
		VehicleItem = NewVehicleItem;

		UE_LOG(LogTemp, Log, TEXT("Added VehicleItem: %s to %s"), *NewVehicleItem->GetName(), *GetName());
	}
	
}

void AHoverVehiclePawn::OnActivateThrottle(const FInputActionValue& value)
{
	bWantsToGoForwardOrBackwards = true;
	const float axisValue = value.Get<float>();
	Speed = FMath::Clamp(axisValue * SpeedMultiplier, 1.0f, MaxSpeed);
}

void AHoverVehiclePawn::OnActivateBrake(const FInputActionValue& value)
{
	bWantsToGoForwardOrBackwards = true;
	Speed *= FMath::Clamp(-1 * BrakeSpeed, -1.0f, -1 * MaxSpeed);
}

void AHoverVehiclePawn::OnActivateHandbrake(const FInputActionValue& value)
{
	OnActivateBrake(value);
}

void AHoverVehiclePawn::OnActivateSteer(const FInputActionValue& value)
{
	//Invert the controls if affected by inverter
	const float axisValue = IsInverted ? -value.Get<float>() : value.Get<float>();
	Steering = SteeringMultiplier * axisValue;

	// Not very clean but if it works
	if (Speed >= 0 && !IsInverted)
	{
		if (axisValue > 0)
		{
			MySteerDirection = ESteerDirection::RIGHT;
		}
		else if (axisValue < 0)
		{
			MySteerDirection = ESteerDirection::LEFT;
		}
		else
		{
			MySteerDirection = ESteerDirection::STRAIGHT;
		}
	}
	else 
	{
		// Reverse if going backwards
		if (axisValue > 0)
		{
			MySteerDirection = ESteerDirection::LEFT;
		}
		else if (axisValue < 0)
		{
			MySteerDirection = ESteerDirection::RIGHT;
		}
		else
		{
			MySteerDirection = ESteerDirection::STRAIGHT;
		}
	}
}

void AHoverVehiclePawn::OnActivateReset(const FInputActionValue& value)
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

void AHoverVehiclePawn::OnActivateUseItem(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();
	
	if (axisValue != 0)
	{
		// TODO: add use item logic
		
		if (VehicleItem != nullptr)
		{
			OnUseItemBP(VehicleItem);
			VehicleItem->UseItem();
		}
	}
}

void AHoverVehiclePawn::OnReleaseThrottle(const FInputActionValue& value)
{
	bWantsToGoForwardOrBackwards = false;
}

void AHoverVehiclePawn::OnReleaseBrake(const FInputActionValue& value)
{
	bWantsToGoForwardOrBackwards = false;
}

void AHoverVehiclePawn::OnReleaseSteer(const FInputActionValue& value)
{
	MySteerDirection = ESteerDirection::STRAIGHT;
}

void AHoverVehiclePawn::RunCameraEffects()
{
	// TODO: Camera shake, motion blur,

	if (EnhancedInputComponent == nullptr)
	{
		return;
	}

	LeanCamera();
	CameraShake();
	ChangeCameraFOV();
}

void AHoverVehiclePawn::LeanCamera()
{
	if (GetVelocity().Length() > FastVelocityThreshold)
	{
		// Workaround for input get value not working
		if (MySteerDirection == ESteerDirection::RIGHT)
		{
			// Lean camera right
			SetLeanSettings(CameraLeanAmount, CameraInterpSpeed);
		}
		else if (MySteerDirection == ESteerDirection::LEFT)
		{
			// Lean camera left
			SetLeanSettings(-1 * CameraLeanAmount, CameraInterpSpeed);
		}
	}
	
	if (MySteerDirection == ESteerDirection::STRAIGHT)
	{
		// Stop lean camera
		SetLeanSettings(0, CameraInterpSpeed);
	}
}

void AHoverVehiclePawn::SetLeanSettings(float Roll, float InterpSpeed)
{
	FRotator currentRotation = Camera->GetRelativeRotation();
	FRotator targetRotation = currentRotation;
	targetRotation.Roll = Roll;
	
	FRotator newRotation = UKismetMathLibrary::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	Camera->SetRelativeRotation(newRotation);
}

void AHoverVehiclePawn::CameraShake()
{
	if (GetVelocity().Length() > FastVelocityThreshold)
	{
		// Done because this is easier in blueprints
		CameraShakeBP();
	}
}

void AHoverVehiclePawn::ChangeCameraFOV()
{
	if (GetVelocity().Length() > FastVelocityThreshold)
	{
		float targetFOV = OriginalFOV * (1 + (SpeedFOVEffect / 1000));
		SetFOVSettings(SpeedFOV, CameraInterpSpeed);
	}
	else
	{
		SetFOVSettings(OriginalFOV, CameraInterpSpeed);
	}
}

void AHoverVehiclePawn::SetFOVSettings(float FOV, float InterpSpeed)
{
	float currentFOV = Camera->FieldOfView;
	float newFOV = UKismetMathLibrary::FInterpTo(currentFOV, FOV, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	Camera->SetFieldOfView(newFOV);
}

void AHoverVehiclePawn::AddCoins()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetActorLocation());
	Coins++;
	if (Coins == 10)
	{
		Boost(3);
		Coins = 0;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Boosted!"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Coins: %d"), Coins));
	}
}