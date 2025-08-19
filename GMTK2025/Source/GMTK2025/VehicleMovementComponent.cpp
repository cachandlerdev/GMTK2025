// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovementComponent.h"
#include "VehiclePawn.h"

/*
This component is responsible for handling the vehicle movement physics. It should only receive physics inputs and resolve them.
*/

// Sets default values for this component's properties
UVehicleMovementComponent::UVehicleMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owner = GetOwner();

		//Start the physics update timer
		owner->GetWorldTimerManager().SetTimer(PhysicsUpdateHandle, this, &UVehicleMovementComponent::UpdateMovementPhysics,
			PhysicsUpdateTime, true);

		Owner = Cast<AVehiclePawn>(owner);

		// Offset center of mass to keep the vehicle upright
		Chassis->SetCenterOfMass(FVector(0.0f, 0.0f, -1 * CenterOfMassOffset));
		/*

	if (owner->GetClass()->ImplementsInterface(UVehicleInterface::StaticClass()))
	{
		Chassis = IVehicleInterface::Execute_GetChassis(owner);

		FrontRightSuspension = IVehicleInterface::Execute_GetFrontRightSuspension(owner);

		FrontLeftSuspension = IVehicleInterface::Execute_GetFrontLeftSuspension(owner);

		BackRightSuspension = IVehicleInterface::Execute_GetBackRightSuspension(owner);

		BackLeftSuspension = IVehicleInterface::Execute_GetBackLeftSuspension(owner);

		


	}
	*/

	// ...
	
}


// Called every frame
void UVehicleMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UVehicleMovementComponent::ApplyMovementForce()
{
	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsMovementFramerateCompensation);
	if (bWantsToGoForwardOrBackwards)
	{
		// Always apply the force parallel to the floor axis.
		FVector force = FVector::VectorPlaneProject(Chassis->GetForwardVector(), FloorSurfaceNormal);

		//force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		//force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;

		// Thanks unreal for making physics framerate dependent
		// TODO: find a less hacky way of doing this
		force.X *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * SpeedMultiplier;
		force.Y *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * SpeedMultiplier;
		force.Z = HoverAmount;

		// Don't let the player get stuck at 0 movement because the controls stop working.
		if (force.X == 0 && force.Y == 0)
		{
			force.X = 1;
			force.Y = 1;
		}

		Chassis->AddForce(force, "", true);
	}
}

void UVehicleMovementComponent::ApplyMovementRotation()
{
	float RotationAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsRotationFramerateCompensation);
	if (SteerDirection != ESteerDirection::STRAIGHT)
	{
		//FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * SteeringMultiplier);

		// Dynamically change the torque amount based on the speed of the car.
		// The slower the car, the stronger the torque. As it speeds up, make the torque weaker.
		float dynamicSteeringStrength = FMath::Abs((1 / Owner->GetVelocity().Length())) * SpeedSteeringFactor;
		dynamicSteeringStrength = FMath::Clamp(dynamicSteeringStrength, MinSteerTorque, MaxSteerTorque);
		FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * RotationAccountForFramerate * dynamicSteeringStrength);
		Chassis->AddTorqueInDegrees(torque, "", true);
	}
}

void UVehicleMovementComponent::ApplySuspension()
{
	UArrowComponent* frontRightSuspension = FrontRightSuspension;
	UArrowComponent* frontLeftSuspension = FrontLeftSuspension;
	UArrowComponent* backRightSuspension = BackRightSuspension;
	UArrowComponent* backLeftSuspension = BackLeftSuspension;

	FVector frontRightStartLocation = frontRightSuspension->GetComponentLocation();
	FVector frontLeftStartLocation = frontLeftSuspension->GetComponentLocation();
	FVector backRightStartLocation = backRightSuspension->GetComponentLocation();
	FVector backLeftStartLocation = backLeftSuspension->GetComponentLocation();

	FVector suspensionDirection = Owner->GetActorUpVector() * -1;

	const float suspensionLength = SuspensionLength;

	FVector frontRightEndLocation = frontRightStartLocation + (suspensionDirection * suspensionLength);
	FVector frontLeftEndLocation = frontLeftStartLocation + (suspensionDirection * suspensionLength);
	FVector backRightEndLocation = backRightStartLocation + (suspensionDirection * suspensionLength);
	FVector backLeftEndLocation = backLeftStartLocation + (suspensionDirection * suspensionLength);

	ApplySuspensionForceOnPoint(frontRightStartLocation, frontRightEndLocation, frontRightSuspension);
	ApplySuspensionForceOnPoint(frontLeftStartLocation, frontLeftEndLocation, frontLeftSuspension);
	ApplySuspensionForceOnPoint(backRightStartLocation, backRightEndLocation, backRightSuspension);
	ApplySuspensionForceOnPoint(backLeftStartLocation, backLeftEndLocation, backLeftSuspension);
}

void UVehicleMovementComponent::ApplyTraction()
{
	// Credit to https://www.youtube.com/watch?v=LG1CtlFRmpU
	// We find the local sideways component of the vehicle's velocity,
	// and apply a counterforce scaled by the traction strength
	float angle = FVector::DotProduct(Owner->GetVelocity(), Owner->GetActorRightVector());
	FVector tractionVectorDirection = Owner->GetActorRightVector() * angle * -1;
	FVector tractionForce = tractionVectorDirection * CurrentTractionStrength;
	Chassis->AddForce(tractionForce, "", true);
}

void UVehicleMovementComponent::ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source)
{	
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Owner);

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
		FVector forceVector = Owner->GetActorUpVector() * force;
		Chassis->AddForceAtLocation(forceVector, StartLocation);
	}

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
}

void UVehicleMovementComponent::StopMovement()
{
	Speed = 0;
	Steering = 0;
	bWantsToGoForwardOrBackwards = false;
	SteerDirection = ESteerDirection::STRAIGHT;

	Chassis->SetSimulatePhysics(false);
	Chassis->SetSimulatePhysics(true);
}

void UVehicleMovementComponent::Throttle(const float axisValue)
{
	bWantsToGoForwardOrBackwards = true;
	Speed = FMath::Clamp(axisValue * SpeedMultiplier, 1.0f, MaxSpeed);
}

void UVehicleMovementComponent::Brake()
{
	bWantsToGoForwardOrBackwards = true;
	Speed *= FMath::Clamp(-1 * BrakeSpeed, -1.0f, -1 * MaxSpeed);
}

void UVehicleMovementComponent::Handbrake()
{
	IsUsingHandbrake = true;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Activate Handbrake."));
	CurrentTractionStrength = 0.0f;
}

void UVehicleMovementComponent::Steer(const float axisValue)
{
	// You can't change steering direction while using the handbrake.
	if (IsUsingHandbrake)
	{
		return;
	}

	//Invert the controls if affected by inverter
	const float finalAxisValue = IsInverted ? -axisValue : axisValue;

	Steering = SteeringMultiplier * finalAxisValue;

	// Not very clean but if it works
	if (Speed >= 0 && !IsInverted)
	{
		if (finalAxisValue > 0)
		{
			SteerDirection = ESteerDirection::RIGHT;
		}
		else if (finalAxisValue < 0)
		{
			SteerDirection = ESteerDirection::LEFT;
		}
		else
		{
			SteerDirection = ESteerDirection::STRAIGHT;
		}
	}
	else
	{
		// Reverse if going backwards
		if (finalAxisValue > 0)
		{
			SteerDirection = ESteerDirection::LEFT;
		}
		else if (finalAxisValue < 0)
		{
			SteerDirection = ESteerDirection::RIGHT;
		}
		else
		{
			SteerDirection = ESteerDirection::STRAIGHT;
		}
	}
}

void UVehicleMovementComponent::ReleaseThrottle()
{
	bWantsToGoForwardOrBackwards = false;
}

void UVehicleMovementComponent::ReleaseBrake()
{
	bWantsToGoForwardOrBackwards = false;
}

void UVehicleMovementComponent::ReleaseHandbrake()
{
	IsUsingHandbrake = false;

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deactivate Handbrake."));
	CurrentTractionStrength = TractionStrength;
}

void UVehicleMovementComponent::ReleaseSteer()
{
	SteerDirection = ESteerDirection::STRAIGHT;
}

float UVehicleMovementComponent::GetCurrentSpeed()
{
	return Speed;
}

float UVehicleMovementComponent::GetCurrentSteering()
{
	return Steering;
}

bool UVehicleMovementComponent::GetCurrentWantsToGoForwardOrBackwards()
{
	return bWantsToGoForwardOrBackwards;
}

double UVehicleMovementComponent::GetCurrentVelocity()
{
	return Owner->GetVelocity().Length();
}

ESteerDirection UVehicleMovementComponent::GetCurrentSteerDirection()
{
	return SteerDirection;
}

bool UVehicleMovementComponent::ShouldApplyMovement()
{
	//Check if the vehicle is EMP'd
	if (IsEMPd)
	{
		return false;
	}

	FHitResult HitResult;
	FVector TraceStart = Owner->GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= MaxDistanceToFloor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

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

	IsOnGround = bHit;

	return bHit;
}

void UVehicleMovementComponent::UpdateMovementPhysics()
{
	if (ShouldApplyMovement())
	{
		ApplyMovementForce();
		ApplyMovementRotation();
		ApplySuspension();
		ApplyTraction();
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Physics Updated From Movement Component."));

	//Store player info to game instance for the ghost
	//Broadcast physics changed
	OnPhysicsUpdated.Broadcast();
}

float UVehicleMovementComponent::GetCurrentVelocityInKMPerHour()
{
	FVector VelocityVector = Owner->GetVelocity();

	double VelocityInCmPerSecond = VelocityVector.Length();

	double VelocityInKMPerHour = VelocityInCmPerSecond * 0.036;

	return VelocityInKMPerHour;
}


const float UVehicleMovementComponent::GetSpeedMultiplier()
{
	return SpeedMultiplier;
}

const float UVehicleMovementComponent::GetMaxSpeed()
{
	return MaxSpeed;
}

const float UVehicleMovementComponent::GetBrakeSpeed()
{
	return BrakeSpeed;
}

const float UVehicleMovementComponent::GetSteeringMultiplier()
{
	return SteeringMultiplier;
}

const float UVehicleMovementComponent::GetTractionStrength()
{
	return TractionStrength;
}

const float UVehicleMovementComponent::GetHoverAmount()
{
	return HoverAmount;
}

const float UVehicleMovementComponent::GetSpeedSteeringFactor()
{
	return SpeedSteeringFactor;
}

const float UVehicleMovementComponent::GetMinSteerTorque()
{
	return MinSteerTorque;
}

const float UVehicleMovementComponent::GetMaxSteerTorque()
{
	return MaxSteerTorque;
}

const float UVehicleMovementComponent::GetSuspensionLength()
{
	return SuspensionLength;
}

const float UVehicleMovementComponent::GetSuspensionStiffness()
{
	return SuspensionStiffness;
}

const float UVehicleMovementComponent::GetSuspensionDamping()
{
	return SuspensionDamping;
}
