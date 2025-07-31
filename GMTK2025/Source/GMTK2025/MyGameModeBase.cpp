// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "Kismet/GameplayStatics.h"

void AMyGameModeBase::InitRaceLogic()
{
	TArray<AActor*> startActors;
	TArray<AActor*> endActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaceStartLocation::StaticClass(), startActors);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARaceEndLocation::StaticClass(), endActors);

	if (CanInitRaceLogic(startActors, endActors))
	{
		StartLocation = Cast<ARaceStartLocation>(startActors[0]);
		EndLocation = Cast<ARaceEndLocation>(endActors[0]);

		if (StartLocation == nullptr || EndLocation == nullptr)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Error: Couldn't get a reference to race start/end locations.")
				);
			}
			return;
		}

		CurrentLoopNumber = 0;
		bHasInitializedRace = true;
	}
}

void AMyGameModeBase::StartNextLoop()
{
	CurrentLoopNumber++;
	APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	player->SetActorLocation(StartLocation->GetActorLocation());
	player->SetActorRotation(StartLocation->GetActorRotation());

	// todo: save player path and create a new ghost
	// reset all ghosts
	// reset coins/items? (tbd)

	OnStartNextLoopBP();
}

void AMyGameModeBase::RestartThisLoop()
{
	if (bHasInitializedRace)
	{
		APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		player->SetActorLocation(StartLocation->GetActorLocation());
		player->SetActorRotation(StartLocation->GetActorRotation());
		OnRestartThisLoopBP();
	}
}

void AMyGameModeBase::FinishThisLoop()
{
	if (bHasInitializedRace)
	{
		// todo: maybe setup some intermediate logic before starting the next loop
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Start next loop"));
		}
		OnFinishThisLoopBP();
	}
}

int32 AMyGameModeBase::GetCurrentLoopNumber()
{
	return CurrentLoopNumber;
}

bool AMyGameModeBase::CanInitRaceLogic(TArray<AActor*> startActors, TArray<AActor*> endActors)
{
	if (startActors.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Error: Couldn't find race start location.")
			);
		}
		return false;
	}
	
	if (endActors.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Error: Couldn't find race start location.")
			);
		}
		return false;
	}
	
	if (startActors.Num() > 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Yellow,TEXT("Warning: Too many race start locations.")
			);
		}
	}
	
	if (endActors.Num() > 1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Yellow,TEXT("Warning: Too many race end locations.")
			);
		}
	}

	return true;
}
