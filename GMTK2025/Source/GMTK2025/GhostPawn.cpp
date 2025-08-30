// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostPawn.h"
#include "Kismet/GameplayStatics.h"


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

	FRotator newRotation = Chassis->GetRelativeRotation();
	newRotation.Roll += DeltaTime * MovementComponent->RotateSpeed;
	Chassis->SetRelativeRotation(newRotation);
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
	Chassis->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);

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

	float MovementAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * MovementComponent->PhysicsMovementFramerateCompensation);
	float RotationAccountForFramerate = 1 / (GetWorld()->GetDeltaSeconds() * MovementComponent->PhysicsRotationFramerateCompensation);

	if (currentWantsForwardOrBackwards)
	{
		FVector force = Chassis->GetForwardVector();
		force.X *= currentSpeed * MovementComponent->SpeedMultiplier * MovementComponent->PhysicsUpdateTime * MovementAccountForFramerate;
		force.Y *= currentSpeed * MovementComponent->SpeedMultiplier * MovementComponent->PhysicsUpdateTime * MovementAccountForFramerate;
		force.Z = MovementComponent->HoverAmount;

		Chassis->AddForce(force, "", true);
	}

	if (currentSteerDirection != ESteerDirection::STRAIGHT)
	{
		FVector torque = FVector(0, 0, currentSteering * MovementComponent->SteeringMultiplier * MovementComponent->PhysicsUpdateTime * RotationAccountForFramerate);
		Chassis->AddTorqueInDegrees(torque, "", true);
	}
	/*
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, TEXT("Applied physics movement."));
	}
	*/
}

void AGhostPawn::ApplyCorrectionFactor(float DeltaTime)
{
	FTransform currentTransform = GetActorTransform();
	FTransform targetTransform = GameInstance->PlayerTransforms[FollowLoopNumber].ArrayOfTransforms[CurrentFollowIndex];

	// Interpolate transform
	FVector NewLocation = FMath::VInterpTo(currentTransform.GetLocation(),
		targetTransform.GetLocation(), DeltaTime,
		GhostPositionInterpolationSpeed);

	// Interpolate scale
	FVector NewScale = FMath::VInterpTo(currentTransform.GetScale3D(), TargetTransform.GetScale3D(),
		DeltaTime, GhostPositionInterpolationSpeed);

	FTransform NewTransform(targetTransform.GetRotation(), NewLocation, NewScale);
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
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Should Update Ghost Location."));
		}
		*/
		ApplyGhostPhysicsMovement(FollowIndex);
	}
}

void AGhostPawn::ReenableCollision()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Reenable collision"));
	}
	Chassis->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
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
				/*
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Should update Ghost location."));
				}
				*/
				UpdateGhostLocation(CurrentFollowIndex);
			}

			//update current transform index
			CurrentFollowIndex++;
		}
		else
		{
			/*
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Array of player positions is missing the required position."));
			}
			*/
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
