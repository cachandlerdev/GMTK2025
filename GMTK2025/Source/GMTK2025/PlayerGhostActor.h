// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HoverVehiclePawn.h"
#include "GameFramework/Actor.h"
#include "MyGameInstance.h"
#include "Components/BoxComponent.h"
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, Category = "Ghost")
	float GhostPositionInterpolationSpeed = 1.0f;
	
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float FollowUpdateForcePhysicsStrength = 5.0f;
	
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float FollowUpdateTorquePhysicsStrength = 3.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

	//Timer
	double GhostSnapshotTimer;

	//Target transform
	FTransform TargetTransform;

	float TargetSpeed;
	float TargetSteering;
	bool bWantsToGoForwardOrBackwards;

	float PlayerMaxDistanceToFloor;

	// Current index of the "copy these player movements" array
	int32 CurrentFollowIndex;

	// Which loop this ghost should copy the inputs for
	int32 FollowLoopNumber;

	//Game instance reference
	UMyGameInstance* GameInstance;

	AHoverVehiclePawn* Player;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Loop")
	void SetFollowLoopNumber(int32 LoopNumber);
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void StartNextLoop(FVector StartLocation);

private:
	void UpdateGhostLocation(int32 FollowIndex);

};
