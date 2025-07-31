// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGhostActor.h"
#include "MyGameInstance.h"

// Sets default values
APlayerGhostActor::APlayerGhostActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	RootComponent = Chassis;
	Chassis->SetSimulatePhysics(false);
	Chassis->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APlayerGhostActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Initiate timer
	GhostSnapshotTimer = GetWorld()->TimeSeconds;

	CurrentTransformIndex = 0;

	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
}

// Called every frame
void APlayerGhostActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update target transform every second
	if (GetWorld()->TimeSeconds - GhostSnapshotTimer >= 1)
	{
		if (GameInstance->PlayerPositions[CurrentTransformIndex].IsValid())
		{
			TargetTransform = GameInstance->PlayerPositions[CurrentTransformIndex];

			//update timer
			GhostSnapshotTimer = GetWorld()->TimeSeconds;

			//update current transform index
			CurrentTransformIndex++;
		}
	}

	//interpolate current ghost transform to target transform
	FTransform CurrentTransform = GetActorTransform();

	// Interpolate Translation
	FVector NewLocation = FMath::VInterpTo(CurrentTransform.GetLocation(), TargetTransform.GetLocation(), DeltaTime, GhostPositionInterpolationSpeed);

	// Interpolate Rotation (using Quaternions for smoother results)
	FQuat NewRotation = FMath::QInterpTo(CurrentTransform.GetRotation(), TargetTransform.GetRotation(), DeltaTime, GhostPositionInterpolationSpeed);

	// Interpolate Scale (if needed)
	FVector NewScale = FMath::VInterpTo(CurrentTransform.GetScale3D(), TargetTransform.GetScale3D(), DeltaTime, GhostPositionInterpolationSpeed);

	// Combine into a new FTransform
	FTransform NewTransform(NewRotation, NewLocation, NewScale);

	//FTransform NewTransform = FMath::FInterpTo(CurrentTransform, TargetTransform, DeltaTime, InterpSpeed);
	SetActorTransform(NewTransform);
}

