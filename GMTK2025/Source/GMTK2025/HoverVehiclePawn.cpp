// Fill out your copyright notice in the Description page of Project Settings.


#include "HoverVehiclePawn.h"
//#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


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

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	//SetRootComponent(Chassis);
	Chassis->SetupAttachment(BoxCollision);
	Chassis->SetSimulatePhysics(true);
	Chassis->SetMassOverrideInKg("", 50000.0);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);

	//Chassis->SetCollisionProfileName(TEXT("Vehicle"));

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(Chassis);
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
}

// Called when the game starts or when spawned
void AHoverVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHoverVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// This is very poor design, but if it works, it works
	FHitResult HitResult;
	FVector TraceStart = GetActorLocation();
	//FVector TraceEnd = TraceStart + GetActorForwardVector() * 1000.0f;
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
		FVector torque = FVector(0, 0, Steering);
		FVector force = Chassis->GetForwardVector();
		force.X *= Speed;
		force.Y *= Speed;
		force.Z = HoverAmount;
		
		BoxCollision->AddForce(force, "", true);
		BoxCollision->AddTorqueInDegrees(torque, "", true);
		
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Apply force"));

		//FVector turnTorque = FVector::CrossProduct(GetActorForwardVector(), FVector(0, 0, 1));

		//turnTorque = turnTorque * DeltaTime * Steering * SteeringMultiplier;
		//
		//turnTorque = FVector::RightVector * DeltaTime * Steering * SteeringMultiplier;
		//if (Steering > 0)
		//{
		//	// Turning right
		//}
		//else
		//{
		//	// Turning left
		//}
		//BoxCollision->AddTorqueInDegrees(turnTorque, "", true);

		float steerAngle = Steering * SteeringVisualRotationMultiplier * DeltaTime;
		steerAngle = FMath::Clamp(steerAngle, -1 * SteeringVisualMaxRotation, SteeringVisualMaxRotation);
		FQuat currentRotation = GetActorQuat();
		//FQuat change(FVector::RightVector, steerAngle);
		FQuat change(Camera->GetForwardVector(), steerAngle);
		FQuat newRotation = currentRotation * change;
		Chassis->SetWorldRotation(newRotation);
		//SetActorRotation(newRotation);

		//float steeringRotationDegrees = FMath::Clamp(Steering * SteeringVisualRotationMultiplier, - 1 * SteeringVisualMaxRotation, SteeringVisualMaxRotation);
		//float angleRadians = FMath::DegreesToRadians(steeringRotationDegrees);
		//FVector rotationAxis = RootComponent->GetForwardVector();
		//FQuat rotationQuat = FQuat(rotationAxis, angleRadians);
		//Chassis->AddTorqueInDegrees(rotationQuat.GetForwardVector(), "", true);
	}

	if (!bIsSteering)
	{
		FVector counterTorque = FVector(0, 0, -1 * Steering);
		BoxCollision->AddTorqueInDegrees(counterTorque, "", true);
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

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateThrottle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AHoverVehiclePawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateHandbrake);
	}
}

void AHoverVehiclePawn::OnActivateThrottle(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();
	Speed = axisValue * SpeedMultiplier;

	//GetVehicleMovementComponent()->SetThrottleInput(CurrentValue);
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Throttle"));
}

void AHoverVehiclePawn::OnActivateBrake(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();
	const float slowAmount = axisValue * SpeedMultiplier * -1;
	Speed -= slowAmount;

	//GetVehicleMovementComponent()->SetBrakeInput(CurrentValue);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Brake"));
}

void AHoverVehiclePawn::OnActivateHandbrake(const FInputActionValue& value)
{
	OnActivateBrake(value);
}

void AHoverVehiclePawn::OnActivateSteer(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();
	Steering = SteeringMultiplier * axisValue;
	
	bIsSteering = true;
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Steer"));
}

void AHoverVehiclePawn::OnReleaseSteer(const FInputActionValue& value)
{
	bIsSteering = false;
}