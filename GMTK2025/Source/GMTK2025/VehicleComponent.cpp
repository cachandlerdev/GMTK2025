// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleComponent.h"
#include "VehicleMovementComponent.h"

/*
This component is responsible for communicating the movement component with the vehicle. It should be home to every variable that is related to the current race and the vehicle.
*/

// Sets default values for this component's properties
UVehicleComponent::UVehicleComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UVehicleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UVehicleComponent::ShouldApplyMovement()
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

void UVehicleComponent::UpdateMovementPhysics()
{
	//TODO: call RecordPlayerInfo from player pawn
	UBoxComponent* VehicleCollisionBox = Vehicle->GetCollisionBox();

	if (ShouldApplyMovement())
	{
		MovementComponent->ApplyMovementForce(Vehicle->GetChassis()->GetForwardVector(), VehicleCollisionBox, FloorSurfaceNormal, Vehicle->GetSpeedMultiplier(), Vehicle->GetHoverAmount());
		MovementComponent->ApplyMovementRotation(Vehicle->GetVelocity().Length(), VehicleCollisionBox, Vehicle->GetSpeedSteeringFactor(), Vehicle->GetMinSteerTorque(), Vehicle->GetMaxSteerTorque());
		MovementComponent->ApplySuspension(Vehicle->GetFrontRightSuspension(), Vehicle->GetFrontLeftSuspension(), Vehicle->GetBackRightSuspension(), Vehicle->GetBackLeftSuspension(),
			Vehicle->GetSuspensionLength(), Vehicle->GetSuspensionStiffness(), Vehicle->GetSuspensionDamping(), Vehicle->GetActorUpVector(), VehicleCollisionBox);
		MovementComponent->ApplyTraction(Vehicle->GetVelocity(), Vehicle->GetActorRightVector(), VehicleCollisionBox);
	}

	//Store player info to game instance for the ghost, every second
	RecordPlayerInfo();
}

float UVehicleComponent::GetCurrentVelocityInKMPerHour()
{
	FVector VelocityVector = Vehicle->GetVelocity();

	double VelocityInCmPerSecond = VelocityVector.Length();

	double VelocityInKMPerHour = VelocityInCmPerSecond * 0.036;

	return VelocityInKMPerHour;
}

float UVehicleComponent::GetCurrentSpeed()
{
	return MovementComponent->GetCurrentSpeed();
}

float UVehicleComponent::GetCurrentSteering()
{
	return MovementComponent->GetCurrentSteering();
}

bool UVehicleComponent::GetCurrentWantsToGoForwardOrBackwards()
{
	return MovementComponent->GetCurrentWantsToGoForwardOrBackwards();
}

ESteerDirection UVehicleComponent::GetCurrentSteerDirection()
{
	return MovementComponent->GetCurrentSteerDirection();
}

AActor* UVehicleComponent::GetVehicle()
{
	return Vehicle;
}

void UVehicleComponent::Throttle(const float axisValue)
{
	MovementComponent->Throttle(axisValue, Vehicle->GetSpeedMultiplier(), Vehicle->GetMaxSpeed());
}

void UVehicleComponent::Brake()
{
	MovementComponent->Brake(Vehicle->GetBrakeSpeed(), Vehicle->GetMaxSpeed());
}

void UVehicleComponent::Handbrake()
{
	IsUsingHandbrake = true;

	MovementComponent->Handbrake();
}

void UVehicleComponent::Steer(const float axisValue)
{
	// You can't change steering direction while using the handbrake.
	if (IsUsingHandbrake)
	{
		return;
	}

	//Invert the controls if affected by inverter
	const float finalAxisValue = IsInverted ? -axisValue : axisValue;

	MovementComponent->Steer(finalAxisValue, Vehicle->GetSteeringMultiplier(), IsInverted);
}

void UVehicleComponent::ReleaseThrottle()
{
	MovementComponent->ReleaseThrottle();
}

void UVehicleComponent::ReleaseBrake()
{
	MovementComponent->ReleaseBrake();
}

void UVehicleComponent::ReleaseHandbrake()
{
	IsUsingHandbrake = false;

	MovementComponent->ReleaseHandbrake(Vehicle->GetTractionStrength());
}

void UVehicleComponent::ReleaseSteer()
{
	MovementComponent->ReleaseSteer();
}