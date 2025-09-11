// Fill out your copyright notice in the Description page of Project Settings.



#include "Child_VehicleItem_Booster.h"
#include "PlayerPawn.h"


void UChild_VehicleItem_Booster::UseItem()
{
	Super::UseItem();
	AActor* Player = GetOwner();

	APlayerPawn* VehiclePawn = Cast<APlayerPawn>(Player);
	if (VehiclePawn)
	{
		VehiclePawn->MovementComponent->Boost(4); //to tweak boost multiplier just change the value in the function here
	}

	RemoveItem();
}

void UChild_VehicleItem_Booster::RemoveItem()
{
	Super::RemoveItem();
}