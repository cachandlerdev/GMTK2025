// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyEnums.h"
#include "Components/ArrowComponent.h"
#include "VehicleMovementComponent.generated.h"

class AVehiclePawn;

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

#pragma region References

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	UStaticMeshComponent* Chassis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* FrontRightSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* FrontLeftSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* BackRightSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* BackLeftSuspension;

#pragma endregion

#pragma region Stats

	//Speed

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SpeedMultiplier = 600.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxSpeed = 4000.0f;

	//TODO: Ask Chris if this belongs to the boost or the vehicle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BoostSpeedMultiplier = 1.2f;

	//TODO: Ask Chris if this belongs to the boost or the vehicle
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float LongBoostUpdateTime = 0.1f;

	// Steering

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float RotateSpeed = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SteeringMultiplier = 800.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SpeedSteeringFactor = 40000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MinSteerTorque = 7.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxSteerTorque = 12.0f;

	// Suspension

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float CenterOfMassOffset = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionLength = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionStiffness = 100000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float SuspensionDamping = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	float TractionStrength = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float HoverAmount = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxDistanceToFloor = 1000.0f;

	// Brake
	// 1.0 lets it stop on a dime, 0 makes it never stop.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BrakeSpeed = 0.5f;

#pragma endregion


	void ApplyMovementForce();

	void ApplyMovementRotation();

	void ApplySuspension();

	// Helps the vehicle grip the road better by applying a traction/slip reduction effect
	void ApplyTraction();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSpeedMultiplier();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetMaxSpeed();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetBrakeSpeed();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSteeringMultiplier();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetTractionStrength();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetHoverAmount();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSpeedSteeringFactor();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetMinSteerTorque();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetMaxSteerTorque();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionLength();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionStiffness();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionDamping();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vehicle")
	float GetCurrentVelocityInKMPerHour();

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

	AVehiclePawn* Owner;

	FTimerHandle PhysicsUpdateHandle;

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
