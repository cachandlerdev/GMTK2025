// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Vehicle.h"
#include "VehicleMovementComponent.h"
#include "VehicleComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK2025_API UVehicleComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UVehicleComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vehicle")
	float GetCurrentVelocityInKMPerHour();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetCurrentSpeed();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetCurrentSteering();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	bool GetCurrentWantsToGoForwardOrBackwards();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	ESteerDirection GetCurrentSteerDirection();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	AActor* GetVehicle();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void Throttle(float axisValue);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void Brake();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void Handbrake();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void Steer(const float axisValue);

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ReleaseThrottle();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ReleaseBrake();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ReleaseHandbrake();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void ReleaseSteer();

private:

	AVehicle* Vehicle;

	UVehicleMovementComponent* MovementComponent;

	//Maybe IsOnGround and FloorSurfaceNormal should be in the movement component

	bool IsOnGround = true;

	// This is the normal vector for the ground beneath the player. Used to ensure force when moving is applied
	// parallel to the ground.
	FVector FloorSurfaceNormal;

	bool IsEMPd = false;

	bool IsInverted = false;

	bool IsUsingHandbrake = false;

	bool ShouldApplyMovement();

	void UpdateMovementPhysics();
};
