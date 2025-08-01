// Fill out your copyright notice in the Description page of Project Settings.


#include "VehicleItems.h"

// Sets default values for this component's properties
UVehicleItems::UVehicleItems()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UVehicleItems::BeginPlay()
{
	Super::BeginPlay();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Vehicle Items Component Initialized"));
	// ...
	
}


// Called every frame
void UVehicleItems::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AActor* PawnOwner = GetOwner();
	FString OwnerName = PawnOwner->GetName();
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, TEXT("Vehicle Items Tick: ") + OwnerName);
	// ...
}

void UVehicleItems::UseItem()
{
	// Implement item usage logic here
	GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Yellow, TEXT("Vehicle Items Tick: "));
}

void UVehicleItems::RemoveItem()
{
	// Implement item removal logic here
}

