// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InventoryComponent.h"
#include "MyGameInstance.h"
#include "MyGameModeBase.h"
#include "VehicleInterface.h"
#include "VehicleMovementComponent.h"
#include "Components/AudioComponent.h"
#include "VehiclePawn.generated.h"

UCLASS()
class GMTK2025_API AVehiclePawn : public APawn, public IVehicleInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVehiclePawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UVehicleMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

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

	UMyGameInstance* GameInstance;

	AMyGameModeBase* GameMode;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaticMeshComponent* GetChassis_Implementation() override;

	UArrowComponent* GetFrontRightSuspension_Implementation() override;

	UArrowComponent* GetFrontLeftSuspension_Implementation() override;

	UArrowComponent* GetBackRightSuspension_Implementation() override;

	UArrowComponent* GetBackLeftSuspension_Implementation() override;

private:

	void LerpChassisToRoot(float DeltaTime);
};
