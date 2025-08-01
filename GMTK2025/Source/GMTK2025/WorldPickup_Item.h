// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup_Base.h"
#include "VehicleItems.h"
#include "WorldPickup_Item.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API AWorldPickup_Item : public APickup_Base
{
	GENERATED_BODY()

public:

	virtual void HandlePickup(AActor* PickupInstigator) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup")
	TSubclassOf<UVehicleItems> VehicleItemClass;
};

