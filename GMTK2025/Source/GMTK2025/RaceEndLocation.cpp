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
	
	//AMyGameModeBase* gamemode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	GameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
}

void ARaceEndLocation::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		APawn* OverlappingPawn = Cast<APawn>(OtherActor);

		if (OverlappingPawn && OverlappingPawn->IsPlayerControlled())
		{
			// The player has reached the end location.
			if (GameMode)
			{
				GameMode->FinishThisLoop();
			}
		}
	}
}

// Called every frame
void ARaceEndLocation::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

