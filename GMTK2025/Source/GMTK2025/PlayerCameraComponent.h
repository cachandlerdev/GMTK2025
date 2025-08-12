// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCameraComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GMTK2025_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerCameraComponent();

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void RunCameraEffects(float DeltaTime);
	void LeanCamera();
	void SetLeanSettings(float Roll, float InterpSpeed);
	void CameraShake();
	void ChangeCameraFOV(float DeltaTime);
	void SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, Category = "Camera")
	void CameraShakeBP();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float OriginalFOV = 90;
};
