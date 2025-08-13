// Fill out your copyright notice in the Description page of Project Settings.


#include "Vehicle.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVehicle::AVehicle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVehicle::BeginPlay()
{
	Super::BeginPlay();
	
	//TODO: Ask Chris why is that needed?
	Chassis->SetWorldLocation(RootComponent->GetComponentLocation());
	Chassis->SetWorldRotation(RootComponent->GetComponentRotation());
}

// Called every frame
void AVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO: Ask Chris why are we doing that
	LerpChassisToRoot(DeltaTime);
}

const float AVehicle::GetSpeedMultiplier()
{
	return SpeedMultiplier;
}

const float AVehicle::GetMaxSpeed()
{
	return MaxSpeed;
}

const float AVehicle::GetBrakeSpeed()
{
	return BrakeSpeed;
}

const float AVehicle::GetSteeringMultiplier()
{
	return SteeringMultiplier;
}

const float AVehicle::GetTractionStrength()
{
	return TractionStrength;
}

const UStaticMeshComponent* AVehicle::GetChassis()
{
	return Chassis;
}

UBoxComponent* AVehicle::GetCollisionBox()
{
	return BoxCollision;
}

const float AVehicle::GetHoverAmount()
{
	return HoverAmount;
}

const float AVehicle::GetSpeedSteeringFactor()
{
	return SpeedSteeringFactor;
}

const float AVehicle::GetMinSteerTorque()
{
	return MinSteerTorque;
}

const float AVehicle::GetMaxSteerTorque()
{
	return MaxSteerTorque;
}

UArrowComponent* AVehicle::GetFrontRightSuspension()
{
	return FrontRightSuspension;
}

UArrowComponent* AVehicle::GetFrontLeftSuspension()
{
	return FrontLeftSuspension;
}

UArrowComponent* AVehicle::GetBackRightSuspension()
{
	return BackRightSuspension;
}

UArrowComponent* AVehicle::GetBackLeftSuspension()
{
	return BackLeftSuspension;
}

const float AVehicle::GetSuspensionLength()
{
	return SuspensionLength;
}

const float AVehicle::GetSuspensionStiffness()
{
	return SuspensionStiffness;
}

const float AVehicle::GetSuspensionDamping()
{
	return SuspensionDamping;
}

void AVehicle::LerpChassisToRoot(float DeltaTime)
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

