// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
#include "MyGameInstance.h"
#include "InventoryComponent.h"
#include "VehicleMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerPawn.generated.h"

class AMyGameModeBase;

UCLASS()
class GMTK2025_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region Inputs

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

	void OnActivateThrottle(const FInputActionValue& value);
	void OnActivateBrake(const FInputActionValue& value);
	void OnActivateHandbrake(const FInputActionValue& value);
	void OnActivateSteer(const FInputActionValue& value);
	void OnActivateReset(const FInputActionValue& value);
	void OnActivateUseItem(const FInputActionValue& value);

	void OnReleaseThrottle(const FInputActionValue& value);
	void OnReleaseBrake(const FInputActionValue& value);
	void OnReleaseHandbrake(const FInputActionValue& value);
	void OnReleaseSteer(const FInputActionValue& value);

	UFUNCTION(BlueprintImplementableEvent, Category = "Pause")
	void OnActivatePauseBP();

#pragma endregion

#pragma region Camera

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraLeanAmount = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraInterpSpeed = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float SpeedFOVEffect = 2.85;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	float FastVelocityThreshold = 3000.0f;

	float OriginalFOV = 90;

	void RunCameraEffects(float DeltaTime);
	void LeanCamera();
	void SetLeanSettings(float Roll, float InterpSpeed);
	void CameraShake();
	void ChangeCameraFOV(float DeltaTime);
	void SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
	void CameraShakeBP();

#pragma endregion

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void RecordPlayerInfo();

private:
	// garbage deletion isn't an issue (I think) because we check if it's null
	// todo: double check this if it's a problem
	UEnhancedInputComponent* EnhancedInputComponent;

	FEnhancedInputActionValueBinding SteeringAxisBinding;

	UVehicleMovementComponent* MovementComponent;

	UInventoryComponent* InventoryComponent;

	UMyGameInstance* GameInstance;

	AMyGameModeBase* GameMode;
};
