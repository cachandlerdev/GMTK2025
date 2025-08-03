// Fill out your copyright notice in the Description page of Project Settings.

#include "Child_PickupType_Coin.h"
#include "HoverVehiclePawn.h"


void AChild_PickupType_Coin::HandlePickup(AActor* PickupInstigator)
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(PickupInstigator);

	if (player)
	{
		player->AddCoins();
	}
	Super::HandlePickup(PickupInstigator);
}
