// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputLibrary.h"
#include "VehicleComponent.h"
#include "PlayerCameraComponent.h"
#include "PlayerPawn.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// garbage deletion isn't an issue (I think) because we check if it's null
	// todo: double check this if it's a problem
	UEnhancedInputComponent* EnhancedInputComponent;

	FEnhancedInputActionValueBinding SteeringAxisBinding;

	//Current Vehicle
	UVehicleComponent* VehicleComponent;

	UPlayerCameraComponent* CameraComponent;
};
