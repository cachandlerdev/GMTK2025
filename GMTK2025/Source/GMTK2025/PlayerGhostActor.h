// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGameInstance.h"
#include "PlayerGhostActor.generated.h"

UCLASS()
class GMTK2025_API APlayerGhostActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerGhostActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	UStaticMeshComponent* Chassis;

	UPROPERTY(EditAnywhere, Category = "Ghost")
	float GhostPositionInterpolationSpeed = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//Timer
	double GhostSnapshotTimer;

	//Target transform
	FTransform TargetTransform;

	//Current index in the player transform array
	int CurrentTransformIndex;

	//Game instance reference
	UMyGameInstance* GameInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
