// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_VehicleItem_Booster.h"
#include "HoverVehiclePawn.h"
#include "GameplayStatics.generated.h"

void UChild_VehicleItem_Booster::UseItem()
{
	
	AActor* Player = GetOwner();

	AHoverVehiclePawn* VehiclePawn = Cast<AHoverVehiclePawn>(Player);
	VehiclePawn->Boost(4); //to tweak boost multiplier just change the value in the function here

	RemoveItem();
}

void UChild_VehicleItem_Booster::RemoveItem()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying Booster component on %s"), *GetOwner()->GetName());
	DestroyComponent();
}