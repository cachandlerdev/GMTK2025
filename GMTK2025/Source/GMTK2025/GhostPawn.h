// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VehiclePawn.h"
#include "GhostPawn.generated.h"

/**
 * 
 */
UCLASS()
class GMTK2025_API AGhostPawn : public AVehiclePawn
{
	GENERATED_BODY()
	
public: 
	// Sets default values for this pawn's properties
	AGhostPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Whether this object is moved via physics or just setting the location.
	UPROPERTY(EditDefaultsOnly, Category = "Ghost")
	bool bUsePlayerPhysicsForMovement = true;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* CollisionMaterial_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* CollisionMaterial_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* NoCollisionMaterial_1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* NoCollisionMaterial_2;

private:

	//Target transform
	FTransform TargetTransform;

	float TargetSpeed;
	float TargetSteering;
	bool bWantsToGoForwardOrBackwards;

	// Current index of the "copy these player movements" array
	int32 CurrentFollowIndex;

	// Which loop this ghost should copy the inputs for
	int32 FollowLoopNumber;

	// Used for temporarily turning collision off
	FTimerHandle CollisionHandle;

	//Used to update physics while we don't refactor the ghost movement
	FTimerHandle PhysicsUpdateHandle;

public:

	UFUNCTION(BlueprintCallable, Category = "Loop")
	void SetFollowLoopNumber(int32 LoopNumber);

	UFUNCTION(BlueprintCallable, Category = "Loop")
	void StartNextLoop(FVector StartLocation, FRotator StartRotation);

	UFUNCTION(BlueprintCallable, Category = "Loop")
	void RestartThisLoop(FVector StartLocation, FRotator StartRotation);

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void OnRestartLoopBP();

	UFUNCTION(BlueprintImplementableEvent, Category = "Vehicle")
	void OnReenableCollisionBP();

private:
	void UpdateGhostLocation(int32 FollowIndex);

	void ReenableCollision();

	void UpdateMovementPhysics();

	bool ShouldUpdateGhostLocation();

	void ApplyGhostPhysicsMovement(int32 FollowIndex);
	void ApplyCorrectionFactor(float DeltaTime);

	bool ShouldApplyCorrectionFactor();
};
