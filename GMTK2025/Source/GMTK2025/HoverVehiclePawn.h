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
#include "NiagaraComponent.h"
#include "VehicleItems.h"
#include "Components/SphereComponent.h"
#include "Components/ArrowComponent.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	USphereComponent* FrontSphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	USphereComponent* BackSphere;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters")
	UNiagaraComponent* RightThrusterParticleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters")
	UNiagaraComponent* LeftThrusterParticleComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* FrontRightSuspensionPoint;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* FrontLeftSuspensionPoint;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* BackRightSuspensionPoint;
	UPROPERTY(BlueprintReadOnly, Category = "Suspension")
	UArrowComponent* BackLeftSuspensionPoint;

	// Sound stuff
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarWindComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarEngineLoopComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineShortBoostSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineLongBoostSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SlowDownPadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* GetCollectableSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxDistanceToFloor = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float SpeedMultiplier = 600.0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float MaxSpeed = 4000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BoostSpeedMultiplier = 1.2f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float LongBoostUpdateTime = 0.1f;

	// Steering torque
	
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
	
	// 1.0 lets it stop on a dime, 0 makes it never stop.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float BrakeSpeed = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float HoverAmount = 20.0f;

	// Lerp chassis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisRotationLerpSpeed = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisXYLerpSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float ChassisZLerpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VehicleLerp")
	float LerpChassisLocationTolerance = 0.1f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float FastVelocityThreshold = 3000.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLeanAmount = 8.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraInterpSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SpeedFOVEffect = 2.85;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float GhostUpdateSeconds = 0.2;

	// How often this vehicle will have force applied (in seconds).
	// Had to switch away from tick to ensure consistent movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsUpdateTime = 0.03333;

	// The larger this value is, the less of a compensation effect we apply to account for framerate dependent physics
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsMovementFramerateCompensation = 60;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float PhysicsRotationFramerateCompensation = 60;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	int32 Coins = 0;

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* PauseAction;

	void OnActivateThrottle(const FInputActionValue &value);
	void OnActivateBrake(const FInputActionValue &value);
	void OnActivateHandbrake(const FInputActionValue &value);
	void OnActivateSteer(const FInputActionValue &value);
	void OnActivateReset(const FInputActionValue &value);
	void OnActivateUseItem(const FInputActionValue &value);
	
	void OnReleaseThrottle(const FInputActionValue &value);
	void OnReleaseBrake(const FInputActionValue &value);
	void OnReleaseHandbrake(const FInputActionValue &value);
	void OnReleaseSteer(const FInputActionValue &value);

	void RunCameraEffects(float DeltaTime);
	void LeanCamera();
	void SetLeanSettings(float Roll, float InterpSpeed);
	void CameraShake();
	void ChangeCameraFOV(float DeltaTime);
	void SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
	void CameraShakeBP();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Pause")
	void OnActivatePauseBP();

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

	//Game instance reference
	UMyGameInstance* GameInstance;

	AMyGameModeBase* GameMode;

	FTimerHandle PhysicsUpdateHandle;
	
	FTimerHandle LongBoostDurationHandle;
	float RemainingLongBoostTime = 0.0f;
	float LongBoostStrengthMultiplier = 1.0f;
	
	bool IsEMPd = false;
	FTimerHandle EMPDurationHandle;

	bool IsInverted = false;
	FTimerHandle InverterDurationHandle;

	bool IsOnGround = true;

	// This is the normal vector for the ground beneath the player. Used to ensure force when moving is applied
	// parallel to the ground.
	FVector FloorSurfaceNormal;

	float OriginalTractionStrength;
	bool IsUsingHandbrake = false;;
	
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
	
	// Disable the vehicle for a certain duration.
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void EMP(float Duration);

	// Invert the steering axis of the vehicle for a certain duration.
	UFUNCTION(BlueprintCallable, Category = "Vehicle")
	void Inverter(float Duration);
	
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item")
	UVehicleItems* VehicleItem;

	UFUNCTION(BlueprintCallable, Category = "Coins")
	void AddCoins();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Items")
	void OnUseItemBP(UVehicleItems* Item);

private:
	bool ShouldApplyMovement();

	void ApplyMovementForce();
	
	void ApplyMovementRotation();

	void RecordPlayerInfo();

	void UpdateMovementPhysics();

	void ApplyLongBoost();

	void EndEMP();

	void EndInverter();
	
	void ApplySuspension();

	// Helps the vehicle grip the road better by applying a traction/slip reduction effect
	void ApplyTraction();

	void ApplySuspensionForceOnPoint(const FVector& StartLocation, const FVector& EndLocation, UArrowComponent* Source);

	void LerpChassisToRoot(float DeltaTime);
};
