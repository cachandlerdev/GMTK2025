// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Vehicle.generated.h"

UCLASS()
class GMTK2025_API AVehicle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVehicle();

	// Visual
#pragma region Visual

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	UStaticMeshComponent* Chassis;

#pragma endregion

	//Suspension
#pragma region Suspension

	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* FrontRightSuspension;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* FrontLeftSuspension;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* BackRightSuspension;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* BackLeftSuspension;

#pragma endregion

	//Collision
#pragma region Collision

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxCollision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	USphereComponent* FrontSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	USphereComponent* BackSphere;

#pragma endregion

	// Sound
#pragma region Sound

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarWindComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarEngineLoopComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineShortBoostSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineLongBoostSound;

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

#pragma region Lerp Chassis
	//TODO: Ask Chris what is this and if it should be on the base vehicle or on the children
	// Lerp chassis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisRotationLerpSpeed = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisXYLerpSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisZLerpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float LerpChassisLocationTolerance = 0.1f;

#pragma endregion

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

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
	const UStaticMeshComponent* GetChassis();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UBoxComponent* GetCollisionBox();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetHoverAmount();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSpeedSteeringFactor();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetMinSteerTorque();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetMaxSteerTorque();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetFrontRightSuspension();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetFrontLeftSuspension();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetBackRightSuspension();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetBackLeftSuspension();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionLength();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionStiffness();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	const float GetSuspensionDamping();

private:

	void LerpChassisToRoot(float DeltaTime);

};
