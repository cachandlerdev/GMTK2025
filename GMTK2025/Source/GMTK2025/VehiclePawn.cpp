// Fill out your copyright notice in the Description page of Project Settings.


#include "VehiclePawn.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVehiclePawn::AVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Chassis

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	SetRootComponent(Chassis);
	Chassis->SetSimulatePhysics(true);
	Chassis->SetCollisionProfileName(TEXT("Vehicle"));
	Chassis->SetCollisionObjectType(ECC_GameTraceChannel1);
	Chassis->BodyInstance.bOverrideMass = true;
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);
	Chassis->SetUsingAbsoluteRotation(true);
	Chassis->SetUsingAbsoluteLocation(true);
	Chassis->SetNotifyRigidBodyCollision(true);

	Chassis->OnComponentHit.AddDynamic(this, &AVehiclePawn::OnComponentHit);	

#pragma endregion

#pragma region Suspension
	
	// Suspension
	FrontRightSuspension = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontRightSuspension"));
	FrontLeftSuspension = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontLeftSuspension"));
	BackRightSuspension = CreateDefaultSubobject<UArrowComponent>(TEXT("BackRightSuspension"));
	BackLeftSuspension = CreateDefaultSubobject<UArrowComponent>(TEXT("BackLeftSuspension"));
	FrontRightSuspension->SetupAttachment(Chassis);
	FrontLeftSuspension->SetupAttachment(Chassis);
	BackRightSuspension->SetupAttachment(Chassis);
	BackLeftSuspension->SetupAttachment(Chassis);

	FrontRightSuspension->SetRelativeLocation(FVector(-185.0f, 80.0f, -50.0f));
	FrontLeftSuspension->SetRelativeLocation(FVector(185.0f, -80.0f, -50.0f));
	BackRightSuspension->SetRelativeLocation(FVector(185.0f, 80.0f, -50.0f));
	BackLeftSuspension->SetRelativeLocation(FVector(-185.0f, -80.0f, -50.0f));

	FrontRightSuspension->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	FrontLeftSuspension->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackRightSuspension->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackLeftSuspension->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	
#pragma endregion

#pragma region Sound
	
	// Audio
	CarWindComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarWindComponent"));
	CarWindComponent->SetupAttachment(RootComponent);
	CarEngineLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarEngineLoopComponent"));
	CarEngineLoopComponent->SetupAttachment(RootComponent);
	
#pragma endregion

	MovementComponent = CreateDefaultSubobject<UVehicleMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->RegisterComponent();

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->RegisterComponent();
}

// Called when the game starts or when spawned
void AVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Game Instance set from Vehicle Pawn."));
		}
	}
}

void AVehiclePawn::OnComponentHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	FVector impactNormal = Hit.ImpactNormal;
	FVector upDirection = FVector(0, 0, 1.0f);
	FVector forwardDirection = GetActorForwardVector();

	bool bIsNotFloor = abs(FVector::DotProduct(impactNormal, upDirection)) < NotFloorCollisionThreshold;
	bool bVehicleMovementIsNotParallelToWall = abs(FVector::DotProduct(impactNormal, forwardDirection)) > NotSidewaysCollisionThreshold;
	bool bWasGoingFastEnough = abs(GetVelocity().Length()) >= MinSpeedForSoundCollision;
	
	if (bIsNotFloor && bVehicleMovementIsNotParallelToWall && bWasGoingFastEnough)
	{
		if (!bHasCollidedRecently)
		{
			bHasCollidedRecently = true;
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), CollisionSound, Hit.Location, GetActorRotation());
			GetWorld()->GetTimerManager().SetTimer(HasCollidedRecentlyHandle, this,
				&AVehiclePawn::ResetHasCollidedRecently, HasCollidedRecentlyCooldown, false);
		}
	}
}

void AVehiclePawn::ResetHasCollidedRecently()
{
	bHasCollidedRecently = false;
}

// Called every frame
void AVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UStaticMeshComponent* AVehiclePawn::GetChassis_Implementation()
{
	return Chassis;
}

UArrowComponent* AVehiclePawn::GetFrontRightSuspension_Implementation()
{
	return FrontRightSuspension;
}

UArrowComponent* AVehiclePawn::GetFrontLeftSuspension_Implementation()
{
	return FrontLeftSuspension;
}

UArrowComponent* AVehiclePawn::GetBackRightSuspension_Implementation()
{
	return BackRightSuspension;
}

UArrowComponent* AVehiclePawn::GetBackLeftSuspension_Implementation()
{
	return BackLeftSuspension;
}
