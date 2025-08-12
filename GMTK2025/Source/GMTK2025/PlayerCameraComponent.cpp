// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraComponent.h"

// Sets default values for this component's properties
UPlayerCameraComponent::UPlayerCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//TODO: Get the root component of the user of the camera component
	//CameraBoom->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);

	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;

	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->SocketOffset.Z = 140.0f;

	OriginalFOV = Camera->FieldOfView;

	// ...
}


// Called when the game starts
void UPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RunCameraEffects(DeltaTime);

	// ...
}

void UPlayerCameraComponent::RunCameraEffects(float DeltaTime)
{
	// TODO: Motion blur

	LeanCamera();
	CameraShake();
	ChangeCameraFOV(DeltaTime);
}

void UPlayerCameraComponent::LeanCamera()
{
	//TODO: get the velocity from the vehicle and the steer direction from the player (or maybe both from the vehicle or the player, whichever is best)

	if (GetVelocity().Length() > FastVelocityThreshold)
	{
		// Workaround for input get value not working
		if (MySteerDirection == ESteerDirection::RIGHT)
		{
			// Lean camera right
			SetLeanSettings(CameraLeanAmount, CameraInterpSpeed);
		}
		else if (MySteerDirection == ESteerDirection::LEFT)
		{
			// Lean camera left
			SetLeanSettings(-1 * CameraLeanAmount, CameraInterpSpeed);
		}
	}

	if (MySteerDirection == ESteerDirection::STRAIGHT)
	{
		// Stop lean camera
		SetLeanSettings(0, CameraInterpSpeed);
	}
}

void UPlayerCameraComponent::SetLeanSettings(float Roll, float InterpSpeed)
{
	FRotator currentRotation = Camera->GetRelativeRotation();
	FRotator targetRotation = currentRotation;
	targetRotation.Roll = Roll;

	FRotator newRotation = UKismetMathLibrary::RInterpTo(currentRotation, targetRotation, GetWorld()->DeltaTimeSeconds, InterpSpeed);
	Camera->SetRelativeRotation(newRotation);
}

void UPlayerCameraComponent::CameraShake()
{
	if (GetVelocity().Length() > FastVelocityThreshold)
	{
		// Done because this is easier in blueprints
		CameraShakeBP();
	}
}

void UPlayerCameraComponent::ChangeCameraFOV(float DeltaTime)
{
	float speed = FMath::Abs(GetVelocity().Length());

	if (speed > FastVelocityThreshold)
	{
		float targetFOV = OriginalFOV * (1 + (SpeedFOVEffect * speed / 100000));
		SetFOVSettings(targetFOV, CameraInterpSpeed, DeltaTime);
	}
	else
	{
		SetFOVSettings(OriginalFOV, CameraInterpSpeed, DeltaTime);
	}
}

void UPlayerCameraComponent::SetFOVSettings(float FOV, float InterpSpeed, float DeltaTime)
{
	float currentFOV = Camera->FieldOfView;
	float newFOV = UKismetMathLibrary::FInterpTo(currentFOV, FOV, DeltaTime, InterpSpeed);
	Camera->SetFieldOfView(newFOV);
}
