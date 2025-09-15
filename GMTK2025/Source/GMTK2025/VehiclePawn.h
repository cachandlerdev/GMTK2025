// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InventoryComponent.h"
#include "MyGameInstance.h"
#include "VehicleInterface.h"
#include "VehicleMovementComponent.h"
#include "Components/AudioComponent.h"
#include "VehiclePawn.generated.h"

UCLASS()
class GMTK2025_API AVehiclePawn : public APawn, public IVehicleInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVehiclePawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UVehicleMovementComponent* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UInventoryComponent* InventoryComponent;

	// Visual
#pragma region Visual

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Vehicle")
	UStaticMeshComponent* Chassis;

#pragma endregion

	//Suspension
#pragma region Suspension
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* FrontRightSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* FrontLeftSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* BackRightSuspension;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Suspension")
	UArrowComponent* BackLeftSuspension;
	
#pragma endregion

	// Sound
#pragma region Sound
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarWindComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	UAudioComponent* CarEngineLoopComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineShortBoostSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* EngineLongBoostSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* CollisionSound;

#pragma endregion


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	UMyGameInstance* GameInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UStaticMeshComponent* GetChassis_Implementation() override;
	
	UArrowComponent* GetFrontRightSuspension_Implementation() override;

	UArrowComponent* GetFrontLeftSuspension_Implementation() override;

	UArrowComponent* GetBackRightSuspension_Implementation() override;

	UArrowComponent* GetBackLeftSuspension_Implementation() override;
	
private:

	// This is the timer used to reset "has collided recently" for sound collisions.
	FTimerHandle HasCollidedRecentlyHandle;
	// This is the minimum amount of time between each sound collision.
	float HasCollidedRecentlyCooldown = 1.0f;
	// This is used as a cooldown to prevent the collision sound effect from playing over and over within a short
	// amount of time.
	bool bHasCollidedRecently = false;
	// This is the minimum threshold we use when comparing the impact normal with the Z direction, and it's used to
	// prevent collisions with the floor from playing a sound. 0.5 allows for collisions that differ by at
	// least 60 degrees, 0.707 allows for ones that differ by 45 degrees, etc. The smaller the value, the more we look
	// at horizontal collisions and ignore vertical ones.
	
	float NotFloorCollisionThreshold = 0.8;
	// This is the minimum threshold we use when comparing the impact normal with the actor's forward vector.
	// 0.707 allows for the impact angle to differ from the "direction the vehicle is going" by 45 degrees, 0.866 is a
	// 30 degree tolerance, etc.
	float NotSidewaysCollisionThreshold = 0.707;
	// This is the minimum speed the vehicle should be going when it crashes for us to play a
	// collision SFX.
	float MinSpeedForSoundCollision = 1500.0f;

	// Used to play a sound effect when the vehicle collides with a wall.
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// Reenables the "can play collision sound" flag
	UFUNCTION()
	void ResetHasCollidedRecently();
};
