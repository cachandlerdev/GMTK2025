// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldPickup_Item.h"

#include "VehiclePawn.h"
#include "Kismet/GameplayStatics.h"

void AWorldPickup_Item::HandlePickup(AActor* PickupInstigator)
{
	AVehiclePawn* player = Cast<AVehiclePawn>(PickupInstigator);

	if (player && player->IsPlayerControlled())
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PickupSound, UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation());

		player->InventoryComponent->AddVehicleItem(VehicleItemClass);
		Destroy();
	}
}
