// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/ArrowComponent.h"
#include "VehicleInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UVehicleInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GMTK2025_API IVehicleInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vehicle")
	UStaticMeshComponent* GetChassis();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetFrontRightSuspension();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetFrontLeftSuspension();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetBackRightSuspension();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vehicle")
	UArrowComponent* GetBackLeftSuspension();
};
