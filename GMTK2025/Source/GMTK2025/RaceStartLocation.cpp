// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceStartLocation.h"

// Sets default values
ARaceStartLocation::ARaceStartLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARaceStartLocation::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARaceStartLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

