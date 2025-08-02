// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_Pickup_Coin.h"
#include "HoverVehiclePawn.h"

void AChild_Pickup_Coin::HandlePickup(AActor* PickupInstigator)
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(PickupInstigator);

	if (player)
	{
		player->IncreasePlayerCoins();
	}
	Super::HandlePickup(PickupInstigator);

}
