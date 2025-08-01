// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPickup_Item.h"
#include "VehicleItems.h"

void AWorldPickup_Item::HandlePickup(AActor* PickupInstigator)
{
	UVehicleItems* ExistingVehicleItems = PickupInstigator->FindComponentByClass<UVehicleItems>();
	
	if (ExistingVehicleItems)
	{
		// Add the item to the vehicle's inventory
		ExistingVehicleItems->RemoveItem();
	}

	UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(PickupInstigator, VehicleItemClass);

	if (NewVehicleItem)
	{
		NewVehicleItem->RegisterComponent();
		NewVehicleItem->UseItem();
	}
	Destroy();
}