// Fill out your copyright notice in the Description page of Project Settings.


#include "HoverVehiclePawn.h"
//#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"


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

	//CameraBoom->bUsePawnControlRotation = false;
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

		//if (GEngine)
		//{
		//	//auto message = FString::Printf(TEXT("Velocity: %f"), GetVelocity().Length());
		//	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, message);
		//}
		
		// Failed attempt for the time being to have the car "tilt in the direction of movement"

		// Attempt 1
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

		// Attempt 2 (somewhat works but looks bad)
		//float steerAngle = Steering * SteeringVisualRotationMultiplier * DeltaTime * -1;
		//steerAngle = FMath::Clamp(steerAngle, -1 * SteeringVisualMaxRotation, SteeringVisualMaxRotation);
		//FQuat currentRotation = GetActorQuat();
		//FQuat change(Camera->GetForwardVector(), steerAngle);
		//FQuat targetRotation = currentRotation * change;
		//FRotator newRotation = UKismetMathLibrary::RLerp(currentRotation.Rotator(), targetRotation.Rotator(), RotationLerp, true);
		//Chassis->SetWorldRotation(newRotation);
		//
		//RotationLerp = FMath::Clamp(RotationLerp + DeltaTime, 0.0f, 1.0f);
		
		//SetActorRotation(newRotation);

		// Attempt 3
		//float steeringRotationDegrees = FMath::Clamp(Steering * SteeringVisualRotationMultiplier, - 1 * SteeringVisualMaxRotation, SteeringVisualMaxRotation);
		//float angleRadians = FMath::DegreesToRadians(steeringRotationDegrees);
		//FVector rotationAxis = RootComponent->GetForwardVector();
		//FQuat rotationQuat = FQuat(rotationAxis, angleRadians);
		//Chassis->AddTorqueInDegrees(rotationQuat.GetForwardVector(), "", true);
	}

	if (MySteerDirection == STRAIGHT)
	{
		//RotationLerp = 0;
		FVector counterTorque = FVector(0, 0, -1 * Steering);
		BoxCollision->AddTorqueInDegrees(counterTorque, "", true);
	}

	RunCameraEffects();
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
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AHoverVehiclePawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::OnActivateHandbrake);

		SteeringAxisBinding = EnhancedInputComponent->BindActionValue(SteeringAction);
	}
}

void AHoverVehiclePawn::OnActivateThrottle(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();
	Speed = axisValue * SpeedMultiplier;
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
	
	// Not very clean but if it works
	if (axisValue > 0)
	{
		MySteerDirection = RIGHT;
	}
	else if (axisValue < 0)
	{
		MySteerDirection = LEFT;
	}
	else
	{
		MySteerDirection = STRAIGHT;
	}
}

void AHoverVehiclePawn::OnReleaseSteer(const FInputActionValue& value)
{
	MySteerDirection = STRAIGHT;
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
		if (MySteerDirection == RIGHT)
		{
			// Lean camera right
			SetLeanSettings(CameraLeanAmount, CameraInterpSpeed);
		}
		else if (MySteerDirection == LEFT)
		{
			// Lean camera left
			SetLeanSettings(-1 * CameraLeanAmount, CameraInterpSpeed);
		}
	}
	
	if (MySteerDirection == STRAIGHT)
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
