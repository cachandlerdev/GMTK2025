// Fill out your copyright notice in the Description page of Project Settings.



#include "Child_VehicleItem_Booster.h"
#include "PlayerPawn.h"
#include "Kismet/GameplayStatics.h"


void UChild_VehicleItem_Booster::UseItem()
{
	Super::UseItem();
	AActor* Player = GetOwner();

	APlayerPawn* VehiclePawn = Cast<APlayerPawn>(Player);
	if (VehiclePawn)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BoostSound, VehiclePawn->GetActorLocation());
		VehiclePawn->MovementComponent->Boost(BoostStrength);
	}

	RemoveItem();
}

void UChild_VehicleItem_Booster::RemoveItem()
{
	Super::RemoveItem();
}