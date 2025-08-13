// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleMovementComponent.h"

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

void UVehicleMovementComponent::ApplyMovementForce(FVector forwardVector, UBoxComponent* forceReceiver, FVector surfaceNormal, const float speedMultiplier, const float hoverAmount)
{
	//TODO: Confirm whether FloorSurfaceNormal should be in the movement component or in the vehicle component

	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsMovementFramerateCompensation);
	if (bWantsToGoForwardOrBackwards)
	{
		// Always apply the force parallel to the floor axis.
		FVector force = FVector::VectorPlaneProject(forwardVector, surfaceNormal);

		//force.X *= Speed * PhysicsUpdateTime * SpeedMultiplier;
		//force.Y *= Speed * PhysicsUpdateTime * SpeedMultiplier;

		// Thanks unreal for making physics framerate dependent
		// TODO: find a less hacky way of doing this
		force.X *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * speedMultiplier;
		force.Y *= Speed * PhysicsUpdateTime * MovementAccountForFramerate * speedMultiplier;
		force.Z = hoverAmount;

		// Don't let the player get stuck at 0 movement because the controls stop working.
		if (force.X == 0 && force.Y == 0)
		{
			force.X = 1;
			force.Y = 1;
		}

		forceReceiver->AddForce(force, "", true);
	}
}

void UVehicleMovementComponent::ApplyMovementRotation(double currentVelocity, UBoxComponent* torqueReceiver, const float speedSteeringFactor, const float minSteerTorque, const float maxSteerTorque)
{
	float RotationAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * PhysicsRotationFramerateCompensation);
	if (SteerDirection != ESteerDirection::STRAIGHT)
	{
		//FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * SteeringMultiplier);

		// Dynamically change the torque amount based on the speed of the car.
		// The slower the car, the stronger the torque. As it speeds up, make the torque weaker.
		float dynamicSteeringStrength = FMath::Abs((1 / currentVelocity)) * speedSteeringFactor;
		dynamicSteeringStrength = FMath::Clamp(dynamicSteeringStrength, minSteerTorque, maxSteerTorque);
		FVector torque = FVector(0, 0, Steering * PhysicsUpdateTime * RotationAccountForFramerate * dynamicSteeringStrength);
		torqueReceiver->AddTorqueInDegrees(torque, "", true);
	}
}

void UVehicleMovementComponent::ApplySuspension(UArrowComponent* frontRightSuspension, UArrowComponent* frontLeftSuspension, UArrowComponent* backRightSuspension, UArrowComponent* backLeftSuspension, 
	const float suspensionLength, const float suspensionStiffness, const float suspensionDamping, FVector upVector, UBoxComponent* suspensionReceiver)
{
	//TODO: get reference from the vehicle for the suspension points and up vector

	FVector frontRightStartLocation = frontRightSuspension->GetComponentLocation();
	FVector frontLeftStartLocation = frontLeftSuspension->GetComponentLocation();
	FVector backRightStartLocation = backRightSuspension->GetComponentLocation();
	FVector backLeftStartLocation = backLeftSuspension->GetComponentLocation();

	FVector suspensionDirection = upVector * -1;

	FVector frontRightEndLocation = frontRightStartLocation + (suspensionDirection * suspensionLength);
	FVector frontLeftEndLocation = frontLeftStartLocation + (suspensionDirection * suspensionLength);
	FVector backRightEndLocation = backRightStartLocation + (suspensionDirection * suspensionLength);
	FVector backLeftEndLocation = backLeftStartLocation + (suspensionDirection * suspensionLength);

	ApplySuspensionForceOnPoint(frontRightStartLocation, frontRightEndLocation, frontRightSuspension, suspensionLength, suspensionStiffness, suspensionDamping, upVector, suspensionReceiver);
	ApplySuspensionForceOnPoint(frontLeftStartLocation, frontLeftEndLocation, frontLeftSuspension, suspensionLength, suspensionStiffness, suspensionDamping, upVector, suspensionReceiver);
	ApplySuspensionForceOnPoint(backRightStartLocation, backRightEndLocation, backRightSuspension, suspensionLength, suspensionStiffness, suspensionDamping, upVector, suspensionReceiver);
	ApplySuspensionForceOnPoint(backLeftStartLocation, backLeftEndLocation, backLeftSuspension, suspensionLength, suspensionStiffness, suspensionDamping, upVector, suspensionReceiver);
}

void UVehicleMovementComponent::ApplyTraction(FVector velocityVector, FVector rightVector, UBoxComponent* tractionReceiver)
{
	//TODO: get reference from the vehicle

	// Credit to https://www.youtube.com/watch?v=LG1CtlFRmpU
	// We find the local sideways component of the vehicle's velocity,
	// and apply a counterforce scaled by the traction strength
	float angle = FVector::DotProduct(velocityVector, rightVector);
	FVector tractionVectorDirection = rightVector * angle * -1;
	FVector tractionForce = tractionVectorDirection * CurrentTractionStrength;
	tractionReceiver->AddForce(tractionForce, "", true);
}

void UVehicleMovementComponent::ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source,
	const float suspensionLength, const float suspensionStiffness, const float suspensionDamping, FVector upVector, UBoxComponent* suspensionReceiver)
{
	//TODO: get reference from the vehicle
	
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
		float compression = 1 - (hitResult.Distance / suspensionLength); // accounts for the -x
		float pointVelocity = Source->GetComponentVelocity().Z;

		float force = (compression * suspensionStiffness) - (suspensionDamping * pointVelocity);
		FVector forceVector = upVector * force;
		suspensionReceiver->AddForceAtLocation(forceVector, StartLocation);
	}

	//DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 1.0f);
}

void UVehicleMovementComponent::StopMovement()
{
	//TODO: get reference from the vehicle

	Speed = 0;
	Steering = 0;
	bWantsToGoForwardOrBackwards = false;
	SteerDirection = ESteerDirection::STRAIGHT;

	BoxCollision->SetSimulatePhysics(false);
	BoxCollision->SetSimulatePhysics(true);
}

void UVehicleMovementComponent::Throttle(const float axisValue, const float speedMultiplier, const float maxSpeed)
{
	bWantsToGoForwardOrBackwards = true;
	Speed = FMath::Clamp(axisValue * speedMultiplier, 1.0f, maxSpeed);
}

void UVehicleMovementComponent::Brake(const float brakeSpeed, const float maxSpeed)
{
	bWantsToGoForwardOrBackwards = true;
	Speed *= FMath::Clamp(-1 * brakeSpeed, -1.0f, -1 * maxSpeed);
}

void UVehicleMovementComponent::Handbrake()
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Activate Handbrake."));
	CurrentTractionStrength = 0.0f;
}

void UVehicleMovementComponent::Steer(const float axisValue, const float steeringMultiplier, const bool shouldInvert)
{
	Steering = steeringMultiplier * axisValue;

	// Not very clean but if it works
	if (Speed >= 0 && !shouldInvert)
	{
		if (axisValue > 0)
		{
			SteerDirection = ESteerDirection::RIGHT;
		}
		else if (axisValue < 0)
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
		if (axisValue > 0)
		{
			SteerDirection = ESteerDirection::LEFT;
		}
		else if (axisValue < 0)
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

void UVehicleMovementComponent::ReleaseHandbrake(const float defaultTractionStrength)
{
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Deactivate Handbrake."));
	CurrentTractionStrength = defaultTractionStrength;
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

ESteerDirection UVehicleMovementComponent::GetCurrentSteerDirection()
{
	return SteerDirection;
}
