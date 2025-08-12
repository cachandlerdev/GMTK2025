// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Vehicle.h"
#include "FirstVehicle.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API AFirstVehicle : public AVehicle
{
	GENERATED_BODY()
	
public: 

	// Sets default values for this actor's properties
	AFirstVehicle();

#pragma region Visual

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters")
	UNiagaraComponent* RightThrusterParticleComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thrusters")
	UNiagaraComponent* LeftThrusterParticleComponent;

#pragma endregion
};
