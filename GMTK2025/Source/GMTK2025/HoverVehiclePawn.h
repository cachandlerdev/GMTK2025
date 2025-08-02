// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputLibrary.h"
#include "Components/BoxComponent.h"
#include "MyEnums.h"
#include "MyGameInstance.h"
#include "VehicleItems.h"
#include "HoverVehiclePawn.generated.h"

class AMyGameModeBase;

class UInputMappingContext;
class UInputAction;

UCLASS()
class GMTK2025_API AHoverVehiclePawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHoverVehiclePawn();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	UStaticMeshComponent* Chassis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxDistanceToFloor = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SpeedMultiplier = 2500.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxSpeed = 4000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SteeringMultiplier = 50.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BoostSpeedMultiplier = 1.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float LongBoostUpdateTime = 0.1f;
	
	// 1.0 lets it stop on a dime, 0 makes it never stop.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BrakeSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float HoverAmount = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float FastVelocityThreshold = 500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLeanAmount = 5.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraInterpSpeed = 5.0f;

	// Divides this number by 500 and adds 1. e.g. "50" becomes a "1.05" FOV change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SpeedFOVEffect = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float GhostUpdateSeconds = 0.2;

	// How often this vehicle will have force applied (in seconds).
	// Had to switch away from tick to ensure consistent movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsUpdateTime = 0.05;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* VehicleMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ThrottleAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* BrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* HandbrakeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SteeringAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAroundAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ResetAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* UseItemAction;

	void OnActivateThrottle(const FInputActionValue &value);
	void OnActivateBrake(const FInputActionValue &value);
	void OnActivateHandbrake(const FInputActionValue &value);
	void OnActivateSteer(const FInputActionValue &value);
	void OnActivateReset(const FInputActionValue &value);
	void OnActivateUseItem(const FInputActionValue &value);
	
	void OnReleaseThrottle(const FInputActionValue &value);
	void OnReleaseBrake(const FInputActionValue &value);
	void OnReleaseSteer(const FInputActionValue &value);

	void RunCameraEffects();
	void LeanCamera();
	void SetLeanSettings(float Roll, float InterpSpeed);
	void CameraShake();
	void ChangeCameraFOV();
	void SetFOVSettings(float FOV, float InterpSpeed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
	void CameraShakeBP();

private:
	float Speed;
	float Steering;
	ESteerDirection MySteerDirection = ESteerDirection::STRAIGHT;
	bool bWantsToGoForwardOrBackwards = false;

	// garbage deletion isn't an issue (I think) because we check if it's null
	// todo: double check this if it's a problem
	UEnhancedInputComponent* EnhancedInputComponent;

	FEnhancedInputActionValueBinding SteeringAxisBinding;
	float OriginalFOV = 90;
	float SpeedFOV = OriginalFOV * (1 + (SpeedFOVEffect / 1000));;

	//Game instance reference
	UMyGameInstance* GameInstance;

	AMyGameModeBase* GameMode;

	UVehicleItems* VehicleItem;

	FTimerHandle PhysicsUpdateHandle;
	
	FTimerHandle LongBoostDurationHandle;
	float RemainingLongBoostTime = 0.0f;
	float LongBoostStrengthMultiplier = 1.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Boost the vehicle forward.
	UFUNCTION(BlueprintCallable, Category="Vehicle")
	void Boost(float BoostStrength);
	
	// Boost the vehicle for a certain duration.
	UFUNCTION(BlueprintCallable, Category="Vehicle")
	void LongBoost(float BoostStrength, float Duration);

	UFUNCTION(BlueprintCallable, Category="Vehicle")
	void StopMovement();
	
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	float GetSpeed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Vehicle")
	float GetCurrentVelocityInKMPerHour();

	//TODO: Change this to an array of pickable items
	UFUNCTION(BlueprintCallable, Category = "Items")
	TArray<int> GetItems();

	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void AddVehicleItem(TSubclassOf<UVehicleItems> VehicleItemClass);
	
private:
	bool ShouldApplyMovement();

	void ApplyPlayerMovement();

	void RecordPlayerInfo();

	void UpdateMovementPhysics();

	void ApplyLongBoost();
};
