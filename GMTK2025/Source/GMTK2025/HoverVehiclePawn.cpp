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


// Sets default values
AHoverVehiclePawn::AHoverVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(BoxCollision);
	Chassis->SetSimulatePhysics(true);
	Chassis->SetMassOverrideInKg("", 50000.0);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);

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
	if (bWantsToGoForwardOrBackwards)
	{
		FVector force = Chassis->GetForwardVector();
		force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		force.Z = HoverAmount;
		
		BoxCollision->AddForce(force, "", true);
	}

	if (MySteerDirection != ESteerDirection::STRAIGHT)
	{
		FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * SteeringMultiplier);
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
	const float baseBoostMultiplier = 100000.0f;
	FVector direction = RootComponent->GetForwardVector();
	BoxCollision->AddForce(direction * BoostStrength * baseBoostMultiplier, "", true);
}

void AHoverVehiclePawn::StopMovement()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Stop movement"));
	
	Speed = 0;
	Steering = 0;
	bWantsToGoForwardOrBackwards = false;
	MySteerDirection = ESteerDirection::STRAIGHT;

	//BoxCollision->SetAllPhysicsLinearVelocity(FVector(0, 0, 0));
	//BoxCollision->SetAllPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
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
	if (IsValid(VehicleItem))
	{
		VehicleItem->RemoveItem();
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
	const float axisValue = value.Get<float>();
	Steering = SteeringMultiplier * axisValue;
	
	// Not very clean but if it works
	if (Speed >= 0)
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
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Reset loop"));

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
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Use item."));
	
	if (axisValue != 0)
	{
		// TODO: add use item logic
		
		//Boost(BoostSpeedMultiplier);
		
		if (IsValid(VehicleItem))
		{
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
