// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPickup_Item.h"

#include "HoverVehiclePawn.h"
#include "VehicleItems.h"
#include "Kismet/GameplayStatics.h"
//****This class handles granting player items when they begin overlapping in the world. Its a subclass of Pickup_Base****
void AWorldPickup_Item::HandlePickup(AActor* PickupInstigator)//Handle pickup item adds item to players inventory then
// calls parents function
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(PickupInstigator);
	

	if (player && player->IsPlayerControlled())// make sure the player is the instigating actor before running function
	{
		player->AddVehicleItem(VehicleItemClass);	
		Super::HandlePickup(PickupInstigator);
	}

	// Replaced for performance reasons (even if it sacrifices portability somewhat)
	
	//UVehicleItems* ExistingVehicleItems = PickupInstigator->FindComponentByClass<UVehicleItems>();
	//
	//if (ExistingVehicleItems)
	//{
	//	// Add the item to the vehicle's inventory
	//	ExistingVehicleItems->RemoveItem();
	//}

	//UVehicleItems* NewVehicleItem = NewObject<UVehicleItems>(PickupInstigator, VehicleItemClass);

	//if (NewVehicleItem)
	//{
	//	NewVehicleItem->RegisterComponent();
	//	NewVehicleItem->UseItem();
	//}
	//Destroy();
}
