// Fill out your copyright notice in the Description page of Project Settings.


#include "Child_VehicleItem_MolecularShift.h"

#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"

void UChild_VehicleItem_MolecularShift::UseItem()
{
	Super::UseItem();
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGhostPawn::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AGhostPawn* GhostPawn = Cast<AGhostPawn>(Actor);
		if (GhostPawn)
		{
			GhostPawn->Chassis->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		}
	}
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &UChild_VehicleItem_MolecularShift::CountUpTimer, 1.0f, true);
	
}
void UChild_VehicleItem_MolecularShift::RemoveItem()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGhostPawn::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		AGhostPawn* GhostPawn = Cast<AGhostPawn>(Actor);
		if (GhostPawn)
		{
			GhostPawn->Chassis->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		}
	}
	GetWorld()->GetTimerManager().ClearTimer(MyTimerHandle);
	Super::RemoveItem();
	
}