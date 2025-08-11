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
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"


// Sets default values
AHoverVehiclePawn::AHoverVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Box collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(135.0f, 85.0f, 85.0f));
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);

	// Offset center of mass to keep the vehicle upright
	BoxCollision->SetCenterOfMass(FVector(0.0f, 0.0f, -1 * CenterOfMassOffset));

	// Chassis
	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(BoxCollision);
	Chassis->SetSimulatePhysics(true);
	Chassis->BodyInstance.bOverrideMass = true;
	Chassis->GetBodyInstance()->SetMassOverride(50000.0, true);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);
	Chassis->SetUsingAbsoluteRotation(true);
	Chassis->SetUsingAbsoluteLocation(true);

	// Suspension
	FrontRightSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontRightSuspensionPoint"));
	FrontLeftSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontLeftSuspensionPoint"));
	BackRightSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BackRightSuspensionPoint"));
	BackLeftSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BackLeftSuspensionPoint"));
	FrontRightSuspensionPoint->SetupAttachment(BoxCollision);
	FrontLeftSuspensionPoint->SetupAttachment(BoxCollision);
	BackRightSuspensionPoint->SetupAttachment(BoxCollision);
	BackLeftSuspensionPoint->SetupAttachment(BoxCollision);

	FrontRightSuspensionPoint->SetRelativeLocation(FVector(-185.0f,80.0f, -50.0f));
	FrontLeftSuspensionPoint->SetRelativeLocation(FVector(185.0f,-80.0f, -50.0f));
	BackRightSuspensionPoint->SetRelativeLocation(FVector(185.0f,80.0f, -50.0f));
	BackLeftSuspensionPoint->SetRelativeLocation(FVector(-185.0f,-80.0f, -50.0f));

	FrontRightSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	FrontLeftSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackRightSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackLeftSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

	// Spheres
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
	
	// Thrusters
	RightThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightThruster"));
	RightThrusterParticleComponent->SetupAttachment(Chassis);
	RightThrusterParticleComponent->SetRelativeLocation(FVector(-224.333333,33.666667,0.333000));
	RightThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	RightThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));
	
	LeftThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftThruster"));
	LeftThrusterParticleComponent->SetupAttachment(Chassis);
	LeftThrusterParticleComponent->SetRelativeLocation(FVector(-224.333333,-29.000000,0.333000));
	LeftThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	LeftThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));

	// Camera
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

	// Audio
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

	Chassis->SetWorldLocation(RootComponent->GetComponentLocation());
	Chassis->SetWorldRotation(RootComponent->GetComponentRotation());
}

// Called every frame
void AHoverVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Make car rotate slightly over time
	//FRotator newRotation = Chassis->GetRelativeRotation();
	//newRotation.Roll += DeltaTime * RotateSpeed;
	//Chassis->SetRelativeRotation(newRotation);
	
	LerpChassisToRoot(DeltaTime);
	
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

	if (bHit)
	{
		FloorSurfaceNormal = HitResult.ImpactNormal;
	}

	return bHit;
}

void AHoverVehiclePawn::ApplyPlayerMovement()
{
	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsMovementFramerateCompensation);
	float RotationAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsRotationFramerateCompensation);
	if (bWantsToGoForwardOrBackwards)
	{
		// Always apply the force parallel to the floor axis.
		FVector forwardVector = Chassis->GetForwardVector();
		FVector force = FVector::VectorPlaneProject(forwardVector, FloorSurfaceNormal);

		//force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		//force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		
		// Thanks unreal for making physics framerate dependent
		// TODO: find a less hacky way of doing this
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
	ApplySuspension();
	
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

void AHoverVehiclePawn::ApplySuspension()
{
	FVector frontRightStartLocation = FrontRightSuspensionPoint->GetComponentLocation();
	FVector frontLeftStartLocation = FrontLeftSuspensionPoint->GetComponentLocation();
	FVector backRightStartLocation = BackRightSuspensionPoint->GetComponentLocation();
	FVector backLeftStartLocation = BackLeftSuspensionPoint->GetComponentLocation();
	
	FVector suspensionDirection = GetActorUpVector() * -1;

	FVector frontRightEndLocation = frontRightStartLocation + (suspensionDirection * SuspensionLength);
	FVector frontLeftEndLocation = frontLeftStartLocation + (suspensionDirection * SuspensionLength);
	FVector backRightEndLocation = backRightStartLocation + (suspensionDirection * SuspensionLength);
	FVector backLeftEndLocation = backLeftStartLocation + (suspensionDirection * SuspensionLength);

	ApplySuspensionForceOnPoint(frontRightStartLocation, frontRightEndLocation, FrontRightSuspensionPoint);
	ApplySuspensionForceOnPoint(frontLeftStartLocation, frontLeftEndLocation, FrontLeftSuspensionPoint);
	ApplySuspensionForceOnPoint(backRightStartLocation, backRightEndLocation, BackRightSuspensionPoint);
	ApplySuspensionForceOnPoint(backLeftStartLocation, backLeftEndLocation, BackLeftSuspensionPoint);
}

void AHoverVehiclePawn::ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source)
{
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		hitResult,
		StartLocation,
		EndLocation,
		ECC_Visibility,
		queryParams
	);

	if (bHit)
	{
		// Credit to https://www.youtube.com/watch?v=LG1CtlFRmpU
		// and https://en.wikipedia.org/wiki/Mass-spring-damper_model
		// Thanks physics guys from several centuries ago!
		
		// Uses the mass-spring-damper model to calculate the force.
		// F = - kx - bv
		// k is the spring's stiffness
		// x is the compression ratio
		// b is the spring damping coefficient
		// v is the velocity at the given point.
		
		// Calculate compression ratio
		float compression = 1 - (hitResult.Distance / SuspensionLength); // accounts for the -x
		float pointVelocity = Source->GetComponentVelocity().Z;
		
		float force = (compression * SuspensionStiffness) - (SuspensionDamping * pointVelocity);
		FVector forceVector = GetActorUpVector() * force;
		BoxCollision->AddForceAtLocation(forceVector, StartLocation);
	}

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
}

void AHoverVehiclePawn::LerpChassisToRoot(float DeltaTime)
{
	// Lerp to root component to make rotation smoother
	FRotator currentRotation = Chassis->GetComponentRotation();
	FRotator targetRotation = BoxCollision->GetComponentRotation();
	FRotator newRotation = UKismetMathLibrary::RLerp(currentRotation, targetRotation, DeltaTime * ChassisRotationLerpSpeed, true);
	Chassis->SetWorldRotation(newRotation);

	// Lerp location as well
	FVector currentLocation = Chassis->GetComponentLocation();
	FVector targetLocation = BoxCollision->GetComponentLocation();
	float newX = FMath::Lerp(currentLocation.X, targetLocation.X, DeltaTime * ChassisXYLerpSpeed);
	float newY = FMath::Lerp(currentLocation.Y, targetLocation.Y, DeltaTime * ChassisXYLerpSpeed);
	float newZ = FMath::Lerp(currentLocation.Z, targetLocation.Z + HoverAmount, DeltaTime * ChassisZLerpSpeed);

	float xToleranceAmount = FMath::Abs(LerpChassisLocationTolerance * RootComponent->GetComponentVelocity().X);
	float yToleranceAmount = FMath::Abs(LerpChassisLocationTolerance * RootComponent->GetComponentVelocity().Y);
	float zToleranceAmount = FMath::Abs(LerpChassisLocationTolerance * RootComponent->GetComponentVelocity().Z);

	newX = FMath::Clamp(newX, currentLocation.X - xToleranceAmount, currentLocation.X + xToleranceAmount);
	newY = FMath::Clamp(newY, currentLocation.Y - yToleranceAmount, currentLocation.Y + yToleranceAmount);
	newZ = FMath::Clamp(newZ, currentLocation.Z - zToleranceAmount, currentLocation.Z + zToleranceAmount);
	
	FVector newLocation = FVector(newX, newY, newZ);
	Chassis->SetWorldLocation(newLocation);
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
	
	UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(this, VehicleItemClass);
	if (NewVehicleItem)
	{
		NewVehicleItem->RegisterComponent();
		VehicleItem = NewVehicleItem;
		
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GetCollectableSound, GetActorLocation());
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