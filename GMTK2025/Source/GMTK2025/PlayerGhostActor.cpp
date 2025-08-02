// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGhostActor.h"
#include "MyGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APlayerGhostActor::APlayerGhostActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxCollision->SetSimulatePhysics(false);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);
	BoxCollision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetSimulatePhysics(false);
	Chassis->SetCollisionProfileName(TEXT("NoCollision"));
	Chassis->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
	Chassis->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Chassis->SetupAttachment(BoxCollision);

	Chassis->BodyInstance.bOverrideMass = true;
	//Chassis->SetMassOverrideInKg("", 50000.0);
	Chassis->GetBodyInstance()->SetMassOverride(50000.0, true);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);
}

// Called when the game starts or when spawned
void APlayerGhostActor::BeginPlay()
{
	Super::BeginPlay();
	
	//Initiate timer
	CurrentFollowIndex = 0;

	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	Player = Cast<AHoverVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	PlayerMaxDistanceToFloor = Player->MaxDistanceToFloor;

	BoxCollision->SetSimulatePhysics(true);
	GetWorldTimerManager().SetTimer(PhysicsUpdateHandle, this, &APlayerGhostActor::UpdateMovementPhysics,
		Player->PhysicsUpdateTime, true);
}

// Called every frame
void APlayerGhostActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldApplyCorrectionFactor())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Apply correction factor"));
		}
		// Correct for movement changes
		ApplyCorrectionFactor(DeltaTime);
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

void APlayerGhostActor::ApplyGhostPhysicsMovement(int32 FollowIndex)
{
	float currentSteering = GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentSpeed = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentWantsForwardOrBackwards = GameInstance->PlayerWantsToGoForwardOrBackwards[FollowLoopNumber].ArrayOfBools[FollowIndex];
	ESteerDirection currentSteerDirection = GameInstance->PlayerSteerDirections[FollowLoopNumber].ArrayOfDirections[FollowIndex];
	
	if (currentWantsForwardOrBackwards)
	{
		FVector force = Chassis->GetForwardVector();
		force.X *= currentSpeed * Player->SpeedMultiplier * Player->PhysicsUpdateTime;
		force.Y *= currentSpeed * Player->SpeedMultiplier * Player->PhysicsUpdateTime;
		force.Z = Player->HoverAmount;
		
		BoxCollision->AddForce(force, "", true);
	}

	if (currentSteerDirection != ESteerDirection::STRAIGHT)
	{
		FVector torque = FVector(0, 0, currentSteering * Player->SteeringMultiplier * Player->PhysicsUpdateTime);
		BoxCollision->AddTorqueInDegrees(torque, "", true);
	}
}

void APlayerGhostActor::ApplyCorrectionFactor(float DeltaTime)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,FString::Printf(TEXT("follow index %i"), CurrentFollowIndex));
	}
	
	FTransform currentTransform = GetActorTransform();
	FTransform targetTransform = GameInstance->PlayerTransforms[FollowLoopNumber].ArrayOfTransforms[CurrentFollowIndex];
	
	// Interpolate transform
	FVector NewLocation = FMath::VInterpTo(currentTransform.GetLocation(),
		targetTransform.GetLocation(), DeltaTime,
		GhostPositionInterpolationSpeed);
	//FVector NewLocation = targetTransform.GetLocation();
	
	//Interpolate rotation (using quaternions for smoother results)
	//FQuat NewRotation = FMath::QInterpTo(currentTransform.GetRotation(), TargetTransform.GetRotation(),
	//	DeltaTime, GhostPositionInterpolationSpeed);
	
	// Interpolate scale
	FVector NewScale = FMath::VInterpTo(currentTransform.GetScale3D(), TargetTransform.GetScale3D(),
		DeltaTime, GhostPositionInterpolationSpeed);
	
	//FTransform NewTransform(NewRotation, NewLocation, NewScale);
	FTransform NewTransform(targetTransform.GetRotation(), NewLocation, NewScale);
	SetActorTransform(NewTransform);
}

bool APlayerGhostActor::ShouldApplyCorrectionFactor()
{
	// Protect against index out of bound issues.

	if (GameInstance == nullptr || GameInstance->PlayerTransforms.Num() == 0 ||
		GameInstance->PlayerTransforms.Num() <= FollowLoopNumber ||
		GameInstance->PlayerTransforms[FollowLoopNumber].ArrayOfTransforms.Num() <= CurrentFollowIndex)
	{
		return false;
	}
	
	FTransform currentTransform = GetActorTransform();
	FTransform targetTransform = GameInstance->PlayerTransforms[FollowLoopNumber].ArrayOfTransforms[CurrentFollowIndex];

	// Check distance
	bool distanceCheck = FVector::Distance(currentTransform.GetLocation(), targetTransform.GetLocation()) > LocationCorrectionFactorThreshold;

	// Check rotation along x axis
	FVector currentXAxis = currentTransform.GetRotation().GetAxisX();
	FVector targetXAxis = targetTransform.GetRotation().GetAxisX();
	float lengths = currentXAxis.Length() + targetXAxis.Length();
	float xRotationDifference = FVector::DotProduct(currentXAxis, targetXAxis) / lengths;
	// xRotationDifference will be 0 if the two are direct opposites
	// 1 if they're exactly the same rotation

	// Make the difference seem more logical by inverting it. 180 degree if opposites, 0 if same
	float xRotationDegreeDifference = 180 - (xRotationDifference * 180);
	
	bool rotationCheck = xRotationDegreeDifference > RotationCorrectionFactorThreshold;

	return distanceCheck || rotationCheck;
}

void APlayerGhostActor::UpdateGhostLocation(int32 FollowIndex)
{
	// Protect against index out of bound issues.
	if (GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats.Num() <= FollowIndex)
	{
		return;
	}
	
	if (ShouldUpdateGhostLocation())
	{
		ApplyGhostPhysicsMovement(FollowIndex);
	}
}

void APlayerGhostActor::ReenableCollision()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Reenable collision"));
	}
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void APlayerGhostActor::UpdateMovementPhysics()
{
	int32 numOfStoredValues = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats.Num();
	if (numOfStoredValues > 0 && numOfStoredValues > CurrentFollowIndex)
	{
		if (bUsePlayerPhysicsForMovement)
		{
			UpdateGhostLocation(CurrentFollowIndex);
		}

		//update current transform index
		CurrentFollowIndex++;
	}
}

bool APlayerGhostActor::ShouldUpdateGhostLocation()
{
	FHitResult HitResult;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= PlayerMaxDistanceToFloor;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	return GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);
}

