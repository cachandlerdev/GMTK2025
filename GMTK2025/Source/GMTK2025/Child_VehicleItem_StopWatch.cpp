// Fill out your copyright notice in the Description page of Project Settings.

#include "Child_VehicleItem_StopWatch.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


void UChild_VehicleItem_StopWatch::BeginPlay()
{
	
}

void UChild_VehicleItem_StopWatch::UseItem()
{
	// Check if the item has not been used yet
	if (bHasNotBeenUsed == true)
	{
		bHasNotBeenUsed = false;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f); // Slow down time to half speed
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandle, this, &UChild_VehicleItem_StopWatch::CountUpTimer, 1.0f, true);
	}
	else
	{
		
	}
}

void UChild_VehicleItem_StopWatch::RemoveItem()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f); // Reset time dilation to normal speed
	Super::RemoveItem();
}

