// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleItems.h"
#include "Child_VehicleItem_RemoteHack.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API UChild_VehicleItem_RemoteHack : public UVehicleItems
{
	GENERATED_BODY()
	
public:
	virtual void UseItem() override;

	virtual void RemoveItem() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Remote Hack")
	float BoostAmount = -8.0f; // Amount to boost the ghost by when using the remote hack item
};
