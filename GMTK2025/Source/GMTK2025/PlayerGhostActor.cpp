// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGhostActor.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlayerGhostActor::APlayerGhostActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetSimulatePhysics(false);
	Chassis->SetupAttachment(BoxCollision);
	
	Chassis->SetMassOverrideInKg("", 50000.0);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);
}

// Called when the game starts or when spawned
void APlayerGhostActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Initiate timer
	GhostSnapshotTimer = GetWorld()->TimeSeconds;

	CurrentFollowIndex = 0;

	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	Player = Cast<AHoverVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	PlayerMaxDistanceToFloor = Player->MaxDistanceToFloor;
}

// Called every frame
void APlayerGhostActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update target transform every second
	if (GetWorld()->TimeSeconds - GhostSnapshotTimer >= Player->GhostUpdateSeconds)
	{
		if (GameInstance->PlayerPositions.Num() > 0 && GameInstance->PlayerPositions.Num() > CurrentFollowIndex)
		{
			if (GameInstance->PlayerPositions[CurrentFollowIndex].IsValid())
			{
				UpdateGhostLocation(CurrentFollowIndex);

				//update timer
				GhostSnapshotTimer = GetWorld()->TimeSeconds;

				//update current transform index
				CurrentFollowIndex++;
			}
		}
	}
}

void APlayerGhostActor::UpdateGhostLocation(int32 FollowIndex)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Update ghost location"));
	}
	
	FHitResult HitResult;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= PlayerMaxDistanceToFloor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// Protect against index out of bound issues.
	if (GameInstance->PlayerSteering.Num() <= FollowIndex)
	{
		return;
	}
	float currentSteering = GameInstance->PlayerSteering[FollowIndex];
	float currentSpeed = GameInstance->PlayerSpeed[FollowIndex];
	bool currentWantsForwardOrBackwards = GameInstance->PlayerWantsToGoForwardOrBackwards[FollowIndex];
	ESteerDirection currentSteerDirection = GameInstance->PlayerSteerDirections[FollowIndex];
	
	if (bHit)
	{
		FVector torque = FVector(0, 0, currentSteering * FollowUpdateTorquePhysicsStrength);

		if (currentWantsForwardOrBackwards)
		{
			FVector force = Chassis->GetForwardVector();
			force.X *= currentSpeed * FollowUpdateForcePhysicsStrength;
			force.Y *= currentSpeed * FollowUpdateForcePhysicsStrength;
			force.Z = Player->HoverAmount;
		
			BoxCollision->AddForce(force, "", true);
		}
		BoxCollision->AddTorqueInDegrees(torque, "", true);
	}

	if (currentSteerDirection == ESteerDirection::STRAIGHT)
	{
		//RotationLerp = 0;
		FVector counterTorque = FVector(0, 0, -1 * currentSteering);
		BoxCollision->AddTorqueInDegrees(counterTorque, "", true);
	}
}

