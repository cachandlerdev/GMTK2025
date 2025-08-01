// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup_Base.h"

// Sets default values
APickup_Base::APickup_Base()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APickup_Base::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickup_Base::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APickup_Base::HandlePickup(AActor* PickupInstigator)
{
	// Logic for handling the pickup
	// This could include adding to inventory, updating score, etc.
	UE_LOG(LogTemp, Warning, TEXT("Pickup handled!"));

	// Optionally destroy the pickup actor after handling
	Destroy();
}

