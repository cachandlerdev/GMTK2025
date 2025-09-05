// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceEndLocation.h"

#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARaceEndLocation::ARaceEndLocation()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f));
	BoxCollision->SetCollisionProfileName(TEXT("OverlapAll"));
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	SetRootComponent(BoxCollision);
}

// Called when the game starts or when spawned
void ARaceEndLocation::BeginPlay()
{
	Super::BeginPlay();

	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARaceEndLocation::OnOverlapBegin);
	}
	
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
}

void ARaceEndLocation::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		APlayerPawn* player = Cast<APlayerPawn>(OtherActor);
		
		if (player && GameMode)
		{
			// This solves a bug where the player collides twice somehow
			if (CanCollide)
			{
				CanCollide = false;
				GetWorldTimerManager().SetTimer(OverlapTimer, this, &ARaceEndLocation::Reenablecollision,
					2.0f, true);	
				GameMode->FinishThisLoop();
			}
			
		}
	}
}

void ARaceEndLocation::Reenablecollision()
{
	CanCollide = true;
}

// Called every frame
void ARaceEndLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

