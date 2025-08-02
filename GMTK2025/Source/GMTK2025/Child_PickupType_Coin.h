// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup_Base.h"
#include "Child_PickupType_Coin.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API AChild_PickupType_Coin : public APickup_Base
{
	GENERATED_BODY()
	
	void HandlePickup(AActor* PickupInstigator) override;
};
