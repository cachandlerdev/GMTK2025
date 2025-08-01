// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleItems.h"
#include "Child_VehicleItem_StopWatch.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API UChild_VehicleItem_StopWatch : public UVehicleItems
{
	GENERATED_BODY()

public:

	virtual void UseItem() override;
	
	virtual void RemoveItem() override;

	virtual void BeginPlay() override;
	
};
