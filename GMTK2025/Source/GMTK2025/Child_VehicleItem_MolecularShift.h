// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehicleItems.h"
#include "Child_VehicleItem_MolecularShift.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API UChild_VehicleItem_MolecularShift : public UVehicleItems
{
	GENERATED_BODY()

public:
	virtual void UseItem() override;

	virtual void RemoveItem() override;

	bool bHasNotBeenUsed = true; // Flag to check if the item has not been used yet

	FTimerHandle MyTimerHandle;

	float effectDuration = 3.0f; // Duration for the time dilation effect


	int32 TimerCount = 0;

	void CountUpTimer()
	{
		TimerCount++;
		if (TimerCount >= effectDuration) // Reset after Specified Duration
		{
			TimerCount = 0;
			GetWorld()->GetTimerManager().ClearTimer(MyTimerHandle);
			RemoveItem();
		}
		else
		{
			// Continue counting up
			GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &UChild_VehicleItem_MolecularShift::CountUpTimer, 1.0f, false);
		}
	}
	
};
