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

	UPROPERTY(EditAnywhere, Category = "Ghost")
	float CollisionOffAfterRestartDuration = 5.0f;

	// Max distance allowed between the ghost's current location and its target "player recorded" location.
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float LocationCorrectionFactorThreshold = 100.0f;

	// Max degree distance allowed between the ghost's current rotation and its target "player recorded" rotation.
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float RotationCorrectionFactorThreshold = 30.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

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

	// Used for temporarily turning collision off
	FTimerHandle CollisionHandle;
	
	FTimerHandle PhysicsUpdateHandle;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category="Loop")
	void SetFollowLoopNumber(int32 LoopNumber);
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void StartNextLoop(FVector StartLocation, FRotator StartRotation);
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void RestartThisLoop(FVector StartLocation, FRotator StartRotation);

private:
	void UpdateGhostLocation(int32 FollowIndex);

	void ReenableCollision();
	
	void UpdateMovementPhysics();

	bool ShouldUpdateGhostLocation();
	
	void ApplyGhostUpdate(int32 FollowIndex);

	void ApplyGhostPhysicsMovement(int32 FollowIndex);
	void ApplyCorrectionFactor(int32 FollowIndex);
	
	bool ShouldApplyCorrectionFactor();
};
