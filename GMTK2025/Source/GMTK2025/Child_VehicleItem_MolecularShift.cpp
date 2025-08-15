// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_VehicleItem_MolecularShift.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerGhostActor.h"

void UChild_VehicleItem_MolecularShift::UseItem()
{
	Super::UseItem();
	TArray<AActor*> FoundActors;// Array to hold found actors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerGhostActor::StaticClass(), FoundActors);// Fills found actors
	//with all the AI ghost cars

	for (AActor* Actor : FoundActors)//sets ai ghost cars to ignore collision with the player
	{
		APlayerGhostActor* GhostActor = Cast<APlayerGhostActor>(Actor);
		if (GhostActor)
		{
			GhostActor->BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
		}
	}
	FTimerHandle TimerHandle;// timer to manage length of the effect
	GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &UChild_VehicleItem_MolecularShift::CountUpTimer, 1.0f, true);
	
}
void UChild_VehicleItem_MolecularShift::RemoveItem()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerGhostActor::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		APlayerGhostActor* GhostActor = Cast<APlayerGhostActor>(Actor);
		if (GhostActor)
		{
			GhostActor->BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);// sets all ghosts to collidable
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(MyTimerHandle);
	Super::RemoveItem();
	
}