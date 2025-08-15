// Fill out your copyright notice in the Description page of Project Settings.

#include "Child_PickupType_Coin.h"
#include "HoverVehiclePawn.h"

//is child class of Base Pickup. This class is used to handle coin pickups for the game
void AChild_PickupType_Coin::HandlePickup(AActor* PickupInstigator)//override of base class function
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(PickupInstigator);

	if (player)// adds coin to player if the instigating actor is the player, then calls parent function
	{
		player->AddCoins();
		Super::HandlePickup(PickupInstigator); Super::HandlePickup(PickupInstigator);
	}
}
