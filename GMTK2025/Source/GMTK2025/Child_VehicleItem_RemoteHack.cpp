// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_VehicleItem_RemoteHack.h"

#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"

void UChild_VehicleItem_RemoteHack::UseItem()
{
	Super::UseItem();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGhostPawn::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AGhostPawn* GhostPawn = Cast<AGhostPawn>(Actor);
		if (GhostPawn)
		{
			GhostPawn->MovementComponent->Boost(BoostAmount);
			FVector ForceDirection = GhostPawn->GetActorRightVector(); // Right direction
			ForceDirection *= 3500.0f; // Tune this for desired impulse

			// Push left or right randomly or based on player input
			int32 Direction = FMath::RandBool() ? 1 : -1;
			ForceDirection *= Direction;

			GhostPawn->Chassis->AddImpulse(ForceDirection, NAME_None, true);
		}
	}
	RemoveItem();
}

void UChild_VehicleItem_RemoteHack::RemoveItem()
{
	Super::RemoveItem();
}