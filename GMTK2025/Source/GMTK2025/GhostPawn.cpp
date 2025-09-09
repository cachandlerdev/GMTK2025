// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerPawn.h"


// Sets default values
AGhostPawn::AGhostPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGhostPawn::BeginPlay()
{
	Super::BeginPlay();

	//Initiate timer
	CurrentFollowIndex = 0;

	APlayerPawn* PlayerPawn = Cast<APlayerPawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	if (PlayerPawn)
	{
		//Copy values from the player to the ghost

		UStaticMesh* chassisMesh = PlayerPawn->Chassis->GetStaticMesh();

		if (chassisMesh)
		{
			Chassis->SetStaticMesh(chassisMesh);
			Chassis->SetRelativeScale3D(PlayerPawn->Chassis->GetRelativeScale3D());
		}

		BackLeftSuspension->SetRelativeLocation(PlayerPawn->BackLeftSuspension->GetRelativeLocation());
		BackRightSuspension->SetRelativeLocation(PlayerPawn->BackRightSuspension->GetRelativeLocation());
		FrontLeftSuspension->SetRelativeLocation(PlayerPawn->FrontLeftSuspension->GetRelativeLocation());
		FrontRightSuspension->SetRelativeLocation(PlayerPawn->FrontRightSuspension->GetRelativeLocation());

		Chassis->SetMassOverrideInKg(NAME_None, PlayerPawn->Chassis->GetMass(), true);

		UVehicleMovementComponent* PlayerMovementComponent = PlayerPawn->MovementComponent;

		MovementComponent->SpeedMultiplier = PlayerMovementComponent->GetSpeedMultiplier();
		MovementComponent->MaxSpeed = PlayerMovementComponent->GetMaxSpeed();
		MovementComponent->BoostSpeedMultiplier = PlayerMovementComponent->BoostSpeedMultiplier;
		MovementComponent->LongBoostUpdateTime = PlayerMovementComponent->LongBoostUpdateTime;
		MovementComponent->SteeringMultiplier = PlayerMovementComponent->GetSteeringMultiplier();
		MovementComponent->SpeedSteeringFactor = PlayerMovementComponent->GetSpeedSteeringFactor();
		MovementComponent->MinSteerTorque = PlayerMovementComponent->GetMinSteerTorque();
		MovementComponent->MaxSteerTorque = PlayerMovementComponent->GetMaxSteerTorque();
		MovementComponent->HoverAmount = PlayerMovementComponent->GetHoverAmount();
		MovementComponent->MaxDistanceToFloor = PlayerMovementComponent->MaxDistanceToFloor;
		MovementComponent->BrakeSpeed = PlayerMovementComponent->GetBrakeSpeed();
		MovementComponent->CenterOfMassOffset = PlayerMovementComponent->CenterOfMassOffset;
		MovementComponent->SuspensionLength = PlayerMovementComponent->GetSuspensionLength();
		MovementComponent->SuspensionStiffness = PlayerMovementComponent->GetSuspensionStiffness();
		MovementComponent->SuspensionDamping = PlayerMovementComponent->GetSuspensionDamping();
		MovementComponent->TractionStrength = PlayerMovementComponent->GetTractionStrength();
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[Ghost] Couldn't find player pawn."));
		}
	}

	GetWorldTimerManager().SetTimer(PhysicsUpdateHandle, this, &AGhostPawn::UpdateMovementPhysics,
		MovementComponent->PhysicsUpdateTime, true);
}

// Called every frame
void AGhostPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ShouldApplyCorrectionFactor())
	{
		// Correct for movement changes
		ApplyCorrectionFactor(DeltaTime);
	}
}

void AGhostPawn::SetFollowLoopNumber(int32 LoopNumber)
{
	FollowLoopNumber = LoopNumber;
}

void AGhostPawn::StartNextLoop(FVector StartLocation, FRotator StartRotation)
{
	RestartThisLoop(StartLocation, StartRotation);
}

void AGhostPawn::RestartThisLoop(FVector StartLocation, FRotator StartRotation)
{
	Chassis->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	GetWorldTimerManager().SetTimer(CollisionHandle, this, &AGhostPawn::ReenableCollision,
		CollisionOffAfterRestartDuration, false);

	Chassis->SetPhysicsLinearVelocity(FVector(0, 0, 0));
	Chassis->SetPhysicsAngularVelocityInDegrees(FVector(0, 0, 0));

	FVector newLocation = StartLocation;
	newLocation.Z += GetDefaultHalfHeight() * 2;
	SetActorLocation(newLocation);
	SetActorRotation(StartRotation);
	Chassis->SetWorldLocation(newLocation);
	Chassis->SetWorldRotation(StartRotation);

	CurrentFollowIndex = 0;
	OnRestartLoopBP();
}

void AGhostPawn::ApplyGhostPhysicsMovement(int32 FollowIndex)
{
	float currentSteering = GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentSpeed = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	float currentWantsForwardOrBackwards = GameInstance->PlayerWantsToGoForwardOrBackwards[FollowLoopNumber].ArrayOfBools[FollowIndex];
	ESteerDirection currentSteerDirection = GameInstance->PlayerSteerDirections[FollowLoopNumber].ArrayOfDirections[FollowIndex];

	MovementComponent->Steering = GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	MovementComponent->Speed = GameInstance->PlayerSpeed[FollowLoopNumber].ArrayOfFloats[FollowIndex];
	MovementComponent->bWantsToGoForwardOrBackwards = GameInstance->PlayerWantsToGoForwardOrBackwards[FollowLoopNumber].ArrayOfBools[FollowIndex];
	MovementComponent->SteerDirection = GameInstance->PlayerSteerDirections[FollowLoopNumber].ArrayOfDirections[FollowIndex];
}

void AGhostPawn::ApplyCorrectionFactor(float DeltaTime)
{
	FTransform currentTransform = GetActorTransform();
	FTransform targetTransform = GameInstance->PlayerTransforms[FollowLoopNumber].ArrayOfTransforms[CurrentFollowIndex];

	// Interpolate transform
	FVector NewLocation = FMath::VInterpTo(currentTransform.GetLocation(),
		targetTransform.GetLocation(), DeltaTime,
		GhostPositionInterpolationSpeed);

	FTransform NewTransform(targetTransform.GetRotation(), NewLocation, GetActorScale3D());

	SetActorTransform(NewTransform);
}

bool AGhostPawn::ShouldApplyCorrectionFactor()
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

void AGhostPawn::UpdateGhostLocation(int32 FollowIndex)
{
	// Protect against index out of bound issues.
	if (GameInstance->PlayerSteering[FollowLoopNumber].ArrayOfFloats.Num() <= FollowIndex)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Location Update Failed: Index out of bounds."));
		}

		return;
	}

	if (ShouldUpdateGhostLocation())
	{
		ApplyGhostPhysicsMovement(FollowIndex);
	}
}

void AGhostPawn::ReenableCollision()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Reenable collision"));
	}
	Chassis->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	OnReenableCollisionBP();
}

void AGhostPawn::UpdateMovementPhysics()
{
	if (!GameInstance)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Game Instance from Ghost invalid."));
		}

		return;
	}
	if (GameInstance->PlayerSpeed.Num() > 0 && GameInstance->PlayerSpeed.Num() >= FollowLoopNumber)
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
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Array of player positions invalid."));
		}
	}
}

bool AGhostPawn::ShouldUpdateGhostLocation()
{
	//Check if the vehicle is EMP'd
	if (MovementComponent->IsEMPd)
	{
		return false;
	}

	FHitResult HitResult;
	FVector TraceStart = GetActorLocation();
	FVector TraceEnd = TraceStart;
	TraceEnd.Z -= MovementComponent->MaxDistanceToFloor;
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
