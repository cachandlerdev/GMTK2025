// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPickup_Item.h"

#include "HoverVehiclePawn.h"
#include "VehicleItems.h"
#include "Kismet/GameplayStatics.h"

void AWorldPickup_Item::HandlePickup(AActor* PickupInstigator)
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(PickupInstigator);
	

	if (player && player->IsPlayerControlled())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());

		player->AddVehicleItem(VehicleItemClass);	
		Destroy();
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
