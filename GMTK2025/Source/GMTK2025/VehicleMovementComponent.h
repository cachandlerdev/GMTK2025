// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyEnums.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "VehicleMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK2025_API UVehicleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleMovementComponent();

	// The larger this value is, the less of a compensation effect we apply to account for framerate dependent physics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsMovementFramerateCompensation = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsRotationFramerateCompensation = 60;

	// How often this vehicle will have force applied (in seconds).
	// Had to switch away from tick to ensure consistent movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsUpdateTime = 0.03333;

	void Throttle(const float axisValue, const float speedMultiplier, const float maxSpeed);

	void Brake(const float brakeSpeed, const float maxSpeed);

	void Handbrake();

	void Steer(const float axisValue, const float steeringMultiplier, const bool shouldInvert);

	void ReleaseThrottle();

	void ReleaseBrake();

	void ReleaseHandbrake(const float defaultTractionStrength);

	void ReleaseSteer();

	void ApplyMovementForce(FVector forwardVector, UBoxComponent* forceReceiver, FVector surfaceNormal, const float speedMultiplier, const float hoverAmount);

	void ApplyMovementRotation(double currentVelocity, UBoxComponent* torqueReceiver, const float speedSteeringFactor, const float minSteerTorque, const float maxSteerTorque);

	void ApplySuspension(UArrowComponent* frontRightSuspension, UArrowComponent* frontLeftSuspension, UArrowComponent* backRightSuspension, UArrowComponent* backLeftSuspension,
		const float suspensionLength, const float suspensionStiffness, const float suspensionDamping, FVector upVector, UBoxComponent* suspensionReceiver);

	// Helps the vehicle grip the road better by applying a traction/slip reduction effect
	void ApplyTraction(FVector velocityVector, FVector rightVector, UBoxComponent* tractionReceiver);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopMovement();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetCurrentSpeed();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetCurrentSteering();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetCurrentWantsToGoForwardOrBackwards();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	ESteerDirection GetCurrentSteerDirection();

private:

	//TODO: Ask Chris what is this for
	bool bWantsToGoForwardOrBackwards = false;

	float Speed;
	float Steering;
	ESteerDirection SteerDirection = ESteerDirection::STRAIGHT;

	float CurrentTractionStrength;

	void ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source,
		const float suspensionLength, const float suspensionStiffness, const float suspensionDamping, FVector upVector, UBoxComponent* suspensionReceiver);
};
