// Fill out your copyright notice in the Description page of Project Settings.


#include "HoverVehiclePawn.h"
#include "ChaosVehicleMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
AHoverVehiclePawn::AHoverVehiclePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
}

// Called when the game starts or when spawned
void AHoverVehiclePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHoverVehiclePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHoverVehiclePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			InputSubsystem->AddMappingContext(VehicleMappingContext, 0);
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::Throttle);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::Brake);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &AHoverVehiclePawn::Steer);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &AHoverVehiclePawn::Handbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &AHoverVehiclePawn::Handbrake);
	}
}

void AHoverVehiclePawn::Throttle(const FInputActionValue& value)
{
	const float CurrentValue = value.Get<float>();

	GetVehicleMovementComponent()->SetThrottleInput(CurrentValue);
	
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Throttle"));
}

void AHoverVehiclePawn::Brake(const FInputActionValue& value)
{
	const float CurrentValue = value.Get<float>();

	GetVehicleMovementComponent()->SetBrakeInput(CurrentValue);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Brake"));
}

void AHoverVehiclePawn::Handbrake(const FInputActionValue& value)
{
	const bool CurrentValue = value.Get<bool>();

	GetVehicleMovementComponent()->SetHandbrakeInput(CurrentValue);
}

void AHoverVehiclePawn::Steer(const FInputActionValue& value)
{
	const float CurrentValue = value.Get<float>();

	GetVehicleMovementComponent()->SetSteeringInput(CurrentValue);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Steer"));
}

