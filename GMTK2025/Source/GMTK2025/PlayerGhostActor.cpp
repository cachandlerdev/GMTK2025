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
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetSimulatePhysics(false);
	Chassis->SetCollisionProfileName(TEXT("NoCollision"));
	Chassis->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	Chassis->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
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
		int32 numOfStoredValues = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats.Num();
		if (numOfStoredValues > 0 && numOfStoredValues > CurrentFollowIndex)
		{
			UpdateGhostLocation(CurrentFollowIndex);

			//update timer
			GhostSnapshotTimer = GetWorld()->TimeSeconds;

			//update current transform index
			CurrentFollowIndex++;
		}
	}
}

void APlayerGhostActor::SetFollowLoopNumber(int32 LoopNumber)
{
	FollowLoopNumber = LoopNumber;
}

void APlayerGhostActor::StartNextLoop(FVector StartLocation, FRotator StartRotation)
{
	RestartThisLoop(StartLocation, StartRotation);
}

void APlayerGhostActor::RestartThisLoop(FVector StartLocation, FRotator StartRotation)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Disable collision"));
	}

	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

	GetWorldTimerManager().SetTimer(CollisionHandle, this, &APlayerGhostActor::ReenableCollision,
		CollisionOffAfterRestartDuration, false);	
	
	BoxCollision->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	BoxCollision->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));
	
	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
	
	CurrentFollowIndex = 0;
}

void APlayerGhostActor::UpdateGhostLocation(int32 FollowIndex)
{
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
	//if (GameInstance->PlayerSteering.Num() <= FollowIndex)
	if (GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats.Num() <= FollowIndex)
	{
		return;
	}
	
	float currentSteering = GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentSpeed = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentWantsForwardOrBackwards = GameInstance->PlayerWantsToGoForwardOrBackwards[FollowLoopNumber].ArrayOfBools[FollowIndex];
	ESteerDirection currentSteerDirection = GameInstance->PlayerSteerDirections[FollowLoopNumber].ArrayOfDirections[FollowIndex];
	
	if (bHit)
	{
		FVector torque = FVector(0, 0, currentSteering * FollowUpdateTorquePhysicsStrength * Player->GhostUpdateSeconds);

		if (currentWantsForwardOrBackwards)
		{
			FVector force = Chassis->GetForwardVector();
			force.X *= currentSpeed * FollowUpdateForcePhysicsStrength * Player->GhostUpdateSeconds;
			force.Y *= currentSpeed * FollowUpdateForcePhysicsStrength * Player->GhostUpdateSeconds;
			force.Z = Player->HoverAmount;
		
			BoxCollision->AddForce(force, "", true);
		}
		BoxCollision->AddTorqueInDegrees(torque, "", true);
	}

	if (currentSteerDirection == ESteerDirection::STRAIGHT)
	{
		//RotationLerp = 0;
		FVector counterTorque = FVector(0, 0, -1 * currentSteering * FollowUpdateTorquePhysicsStrength * Player->GhostUpdateSeconds);
		BoxCollision->AddTorqueInDegrees(counterTorque, "", true);
	}
}

void APlayerGhostActor::ReenableCollision()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Reenable collision"));
	}
	//ReenableLoopCollisionBP();
	//BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
	
	//BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	//FCollisionResponseContainer CurrentResponses = BoxCollision->GetCollisionResponseToChannels();
	//CurrentResponses.SetResponse(ECC_GameTraceChannel1, ECR_Block);
	//BoxCollision->SetCollisionResponseToChannels(CurrentResponses);
}

