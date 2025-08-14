// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyEnums.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Vehicle.h"
#include "VehicleMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK2025_API UVehicleMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	//Event called when physics change
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhysicsUpdated);
	UPROPERTY(BlueprintAssignable, Category = "Movement")
	FOnPhysicsUpdated OnPhysicsUpdated;

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
	/*
	void Throttle(const float axisValue);

	void Brake();

	void Handbrake();

	void Steer(const float axisValue);

	void ReleaseThrottle();

	void ReleaseBrake();

	void ReleaseHandbrake();

	void ReleaseSteer();
	*/
	void ApplyMovementForce();

	void ApplyMovementRotation();

	void ApplySuspension();

	// Helps the vehicle grip the road better by applying a traction/slip reduction effect
	void ApplyTraction();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vehicle")
	float GetCurrentVelocityInKMPerHour();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	AActor* GetVehicle();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Throttle(float axisValue);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Brake();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Handbrake();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void Steer(const float axisValue);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ReleaseThrottle();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ReleaseBrake();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ReleaseHandbrake();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ReleaseSteer();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void StopMovement();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetCurrentSpeed();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	float GetCurrentSteering();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	bool GetCurrentWantsToGoForwardOrBackwards();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	double GetCurrentVelocity();

	UFUNCTION(BlueprintCallable, Category = "Movement")
	ESteerDirection GetCurrentSteerDirection();

private:

	AVehicle* Vehicle;

	//TODO: Ask Chris what is this for
	bool bWantsToGoForwardOrBackwards = false;

	float Speed;
	float Steering;
	ESteerDirection SteerDirection = ESteerDirection::STRAIGHT;

	float CurrentTractionStrength;

	bool IsOnGround = true;

	// This is the normal vector for the ground beneath the player. Used to ensure force when moving is applied
	// parallel to the ground.
	FVector FloorSurfaceNormal;

	bool IsEMPd = false;

	bool IsInverted = false;

	bool IsUsingHandbrake = false;

	bool ShouldApplyMovement();

	void UpdateMovementPhysics();

	void ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source);
};
