// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovementComponent.h"
#include "PlayerPawn.h"

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
	//TODO: Confirm whether FloorSurfaceNormal should be in the movement component or in the vehicle component

	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsMovementFramerateCompensation);
	if (bWantsToGoForwardOrBackwards)
	{
		// Always apply the force parallel to the floor axis.
		FVector force = FVector::VectorPlaneProject(Vehicle->GetChassis()->GetForwardVector(), FloorSurfaceNormal);

		//force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		//force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;

		// Thanks unreal for making physics framerate dependent
		// TODO: find a less hacky way of doing this
		force.X *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * Vehicle->GetSpeedMultiplier();
		force.Y *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * Vehicle->GetSpeedMultiplier();
		force.Z = Vehicle->GetHoverAmount();

		// Don't let the player get stuck at 0 movement because the controls stop working.
		if (force.X == 0 && force.Y == 0)
		{
			force.X = 1;
			force.Y = 1;
		}

		Vehicle->GetCollisionBox()->AddForce(force, "", true);
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
		float dynamicSteeringStrength = FMath::Abs((1 / Vehicle->GetVelocity().Length())) * Vehicle->GetSpeedSteeringFactor();
		dynamicSteeringStrength = FMath::Clamp(dynamicSteeringStrength, Vehicle->GetMinSteerTorque(), Vehicle->GetMaxSteerTorque());
		FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * RotationAccountForFramerate * dynamicSteeringStrength);
		Vehicle->GetCollisionBox()->AddTorqueInDegrees(torque, "", true);
	}
}

void UVehicleMovementComponent::ApplySuspension()
{
	UArrowComponent* frontRightSuspension = Vehicle->GetFrontRightSuspension();
	UArrowComponent* frontLeftSuspension = Vehicle->GetFrontLeftSuspension();
	UArrowComponent* backRightSuspension = Vehicle->GetBackRightSuspension();
	UArrowComponent* backLeftSuspension = Vehicle->GetBackLeftSuspension();

	FVector frontRightStartLocation = frontRightSuspension->GetComponentLocation();
	FVector frontLeftStartLocation = frontLeftSuspension->GetComponentLocation();
	FVector backRightStartLocation = backRightSuspension->GetComponentLocation();
	FVector backLeftStartLocation = backLeftSuspension->GetComponentLocation();

	FVector suspensionDirection = Vehicle->GetActorUpVector() * -1;

	const float suspensionLength = Vehicle->GetSuspensionLength();

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
	float angle = FVector::DotProduct(Vehicle->GetVelocity(), Vehicle->GetActorRightVector());
	FVector tractionVectorDirection = Vehicle->GetActorRightVector() * angle * -1;
	FVector tractionForce = tractionVectorDirection * CurrentTractionStrength;
	Vehicle->GetCollisionBox()->AddForce(tractionForce, "", true);
}

void UVehicleMovementComponent::ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source)
{	
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(Vehicle);

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
		float compression = 1 - (hitResult.Distance / Vehicle->GetSuspensionLength()); // accounts for the -x
		float pointVelocity = Source->GetComponentVelocity().Z;

		float force = (compression * Vehicle->GetSuspensionStiffness()) - (Vehicle->GetSuspensionDamping() * pointVelocity);
		FVector forceVector = Vehicle->GetActorUpVector() * force;
		Vehicle->GetCollisionBox()->AddForceAtLocation(forceVector, StartLocation);
	}

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
}

void UVehicleMovementComponent::StopMovement()
{
	Speed = 0;
	Steering = 0;
	bWantsToGoForwardOrBackwards = false;
	SteerDirection = ESteerDirection::STRAIGHT;

	Vehicle->GetCollisionBox()->SetSimulatePhysics(false);
	Vehicle->GetCollisionBox()->SetSimulatePhysics(true);
}

void UVehicleMovementComponent::Throttle(const float axisValue)
{
	bWantsToGoForwardOrBackwards = true;
	Speed = FMath::Clamp(axisValue * Vehicle->GetSpeedMultiplier(), 1.0f, Vehicle->GetMaxSpeed());
}

void UVehicleMovementComponent::Brake()
{
	bWantsToGoForwardOrBackwards = true;
	Speed *= FMath::Clamp(-1 * Vehicle->GetBrakeSpeed(), -1.0f, -1 * Vehicle->GetMaxSpeed());
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

	Steering = Vehicle->GetSteeringMultiplier() * finalAxisValue;

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
	CurrentTractionStrength = Vehicle->GetTractionStrength();
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
	return Vehicle->GetVelocity().Length();
}

ESteerDirection UVehicleMovementComponent::GetCurrentSteerDirection()
{
	return SteerDirection;
}

AActor* UVehicleMovementComponent::GetVehicle()
{
	return Vehicle;
}

bool UVehicleMovementComponent::ShouldApplyMovement()
{
	//Check if the vehicle is EMP'd
	if (IsEMPd)
	{
		return false;
	}

	FHitResult HitResult;
	FVector TraceStart = Vehicle->GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= Vehicle->MaxDistanceToFloor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Vehicle);

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

	//Store player info to game instance for the ghost
	//Broadcast physics changed
	OnPhysicsUpdated.Broadcast();
}

float UVehicleMovementComponent::GetCurrentVelocityInKMPerHour()
{
	FVector VelocityVector = Vehicle->GetVelocity();

	double VelocityInCmPerSecond = VelocityVector.Length();

	double VelocityInKMPerHour = VelocityInCmPerSecond * 0.036;

	return VelocityInKMPerHour;
}

