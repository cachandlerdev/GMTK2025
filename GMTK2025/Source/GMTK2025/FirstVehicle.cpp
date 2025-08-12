// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstVehicle.h"

// Sets default values
AFirstVehicle::AFirstVehicle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Collision

	// Box collision
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(135.0f, 85.0f, 85.0f));
	BoxCollision->SetSimulatePhysics(true);
	BoxCollision->SetCollisionProfileName(TEXT("Vehicle"));
	BoxCollision->SetCollisionObjectType(ECC_GameTraceChannel1);

	// Spheres
	FrontSphere = CreateDefaultSubobject<USphereComponent>(TEXT("FrontSphereCollision"));
	FrontSphere->SetSphereRadius(90.0f);
	FrontSphere->SetupAttachment(BoxCollision);
	FrontSphere->SetGenerateOverlapEvents(false);
	FrontSphere->SetRelativeLocation(FVector(106.0f, 0.0f, 0.0f));

	BackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("BackSphereCollision"));
	BackSphere->SetSphereRadius(90.0f);
	BackSphere->SetupAttachment(BoxCollision);
	BackSphere->SetGenerateOverlapEvents(false);
	BackSphere->SetRelativeLocation(FVector(-133.0f, 0.0f, 0.0f));

#pragma endregion

#pragma region Visual

	// Chassis
	Chassis = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Chassis"));
	Chassis->SetupAttachment(BoxCollision);
	Chassis->SetSimulatePhysics(true);
	Chassis->BodyInstance.bOverrideMass = true;
	Chassis->GetBodyInstance()->SetMassOverride(50000.0, true);
	Chassis->SetLinearDamping(1.0);
	Chassis->SetAngularDamping(1.0);
	Chassis->SetUsingAbsoluteRotation(true);
	Chassis->SetUsingAbsoluteLocation(true);

	// Thrusters
	RightThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RightThruster"));
	RightThrusterParticleComponent->SetupAttachment(Chassis);
	RightThrusterParticleComponent->SetRelativeLocation(FVector(-224.333333, 33.666667, 0.333000));
	RightThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	RightThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));

	LeftThrusterParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LeftThruster"));
	LeftThrusterParticleComponent->SetupAttachment(Chassis);
	LeftThrusterParticleComponent->SetRelativeLocation(FVector(-224.333333, -29.000000, 0.333000));
	LeftThrusterParticleComponent->SetRelativeRotation(FRotator(180, 0, 0));
	LeftThrusterParticleComponent->SetRelativeScale3D(FVector(0.5));

#pragma endregion

#pragma region Suspension

	// Offset center of mass to keep the vehicle upright
	BoxCollision->SetCenterOfMass(FVector(0.0f, 0.0f, -1 * CenterOfMassOffset));

	// Suspension
	FrontRightSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontRightSuspensionPoint"));
	FrontLeftSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontLeftSuspensionPoint"));
	BackRightSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BackRightSuspensionPoint"));
	BackLeftSuspensionPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("BackLeftSuspensionPoint"));
	FrontRightSuspensionPoint->SetupAttachment(BoxCollision);
	FrontLeftSuspensionPoint->SetupAttachment(BoxCollision);
	BackRightSuspensionPoint->SetupAttachment(BoxCollision);
	BackLeftSuspensionPoint->SetupAttachment(BoxCollision);

	FrontRightSuspensionPoint->SetRelativeLocation(FVector(-185.0f, 80.0f, -50.0f));
	FrontLeftSuspensionPoint->SetRelativeLocation(FVector(185.0f, -80.0f, -50.0f));
	BackRightSuspensionPoint->SetRelativeLocation(FVector(185.0f, 80.0f, -50.0f));
	BackLeftSuspensionPoint->SetRelativeLocation(FVector(-185.0f, -80.0f, -50.0f));

	FrontRightSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	FrontLeftSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackRightSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	BackLeftSuspensionPoint->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

#pragma endregion
	
#pragma region Sound

	// Audio
	CarWindComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarWindComponent"));
	CarWindComponent->SetupAttachment(RootComponent);
	CarEngineLoopComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("CarEngineLoopComponent"));
	CarEngineLoopComponent->SetupAttachment(RootComponent);

#pragma endregion

}