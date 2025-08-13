// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameInstance.h"
#include "MyGameModeBase.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	GameInstance = Cast<UMyGameInstance>(GetGameInstance());
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			InputSubsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* MyInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent = MyInputComponent;
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseThrottle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseBrake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivateHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseHandbrake);

		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivateSteer);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Completed, this, &APlayerPawn::OnReleaseSteer);

		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerPawn::OnActivatePauseBP);

		EnhancedInputComponent->BindAction(ResetAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateReset);
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Triggered, this, &APlayerPawn::OnActivateUseItem);

		SteeringAxisBinding = EnhancedInputComponent->BindActionValue(SteeringAction);
	}
}

void APlayerPawn::RecordPlayerInfo()
{
	int32 loopNum = GameMode->GetCurrentLoopNumber();
	
	if (loopNum > -1)
	{
		GameInstance->PlayerSpeed[loopNum].ArrayOfFloats.Emplace(VehicleComponent->GetCurrentSpeed());
		GameInstance->PlayerSteering[loopNum].ArrayOfFloats.Emplace(VehicleComponent->GetCurrentSteering());
		GameInstance->PlayerWantsToGoForwardOrBackwards[loopNum].ArrayOfBools.Emplace(VehicleComponent->GetCurrentWantsToGoForwardOrBackwards());
		GameInstance->PlayerSteerDirections[loopNum].ArrayOfDirections.Emplace(VehicleComponent->GetCurrentSteerDirection());

		GameInstance->PlayerTransforms[loopNum].ArrayOfTransforms.Emplace(VehicleComponent->GetVehicle()->GetActorTransform());
	}
}

void APlayerPawn::OnActivateReset(const FInputActionValue& value)
{
	const float axisValue = value.Get<float>();

	if (axisValue != 0)
	{
		if (GameMode)
		{
			GameMode->RestartThisLoop();
		}
	}
}

void APlayerPawn::OnActivateUseItem(const FInputActionValue& value)
{
	InventoryComponent->UseItem(value.Get<float>());
}

void APlayerPawn::OnActivateThrottle(const FInputActionValue& value)
{
	VehicleComponent->Throttle(value.Get<float>());
}

void APlayerPawn::OnActivateBrake(const FInputActionValue& value)
{
	VehicleComponent->Brake();
}

void APlayerPawn::OnActivateHandbrake(const FInputActionValue& value)
{
	VehicleComponent->Handbrake();
}

void APlayerPawn::OnActivateSteer(const FInputActionValue& value)
{
	VehicleComponent->Steer(value.Get<float>());
}

void APlayerPawn::OnReleaseThrottle(const FInputActionValue& value)
{
	VehicleComponent->ReleaseThrottle();
}

void APlayerPawn::OnReleaseBrake(const FInputActionValue& value)
{
	VehicleComponent->ReleaseBrake();
}

void APlayerPawn::OnReleaseHandbrake(const FInputActionValue& value)
{
	VehicleComponent->ReleaseHandbrake();
}

void APlayerPawn::OnReleaseSteer(const FInputActionValue& value)
{
	VehicleComponent->ReleaseSteer();
}