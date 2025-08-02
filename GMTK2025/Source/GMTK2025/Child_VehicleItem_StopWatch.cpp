// Fill out your copyright notice in the Description page of Project Settings.

#include "Child_VehicleItem_StopWatch.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


void UChild_VehicleItem_StopWatch::BeginPlay()
{
	UChild_VehicleItem_StopWatch::UseItem();
}

void UChild_VehicleItem_StopWatch::UseItem()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.5f); // Slow down time to half speed

}

void UChild_VehicleItem_StopWatch::RemoveItem()
{
	DestroyComponent();
}