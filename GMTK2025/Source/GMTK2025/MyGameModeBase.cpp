// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

#include "PlayerGhostActor.h"
#include "Kismet/GameplayStatics.h"

void AMyGameModeBase::InitRaceLogic()
{
	GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	if (!GameInstance)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Error: Couldn't get game instance."));
		}
	}
	
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

		CurrentLoopNumber = -1;
		bHasInitializedRace = true;
	}
}

void AMyGameModeBase::StartNextLoop()
{
	CurrentLoopNumber++;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,FString::Printf(TEXT("Start loop %i"), CurrentLoopNumber));
	}
	
	APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	player->SetActorLocation(StartLocation->GetActorLocation());
	player->SetActorRotation(StartLocation->GetActorRotation());

	// Add the data arrays to track this loop
	FInnerFloatArray speedThisLoop;
	FInnerFloatArray steeringThisLoop;
	FInnerBoolArray wantsToGoForwardOrBackwardsThisLoop;
	FInnerSteerDirectionArray steerDirectionThisLoop;
	
	GameInstance->PlayerSpeed.Add(speedThisLoop);
	GameInstance->PlayerSteering.Add(steeringThisLoop);
	GameInstance->PlayerWantsToGoForwardOrBackwards.Add(wantsToGoForwardOrBackwardsThisLoop);
	GameInstance->PlayerSteerDirections.Add(steerDirectionThisLoop);

	for (int32 i = 0; i < Ghosts.Num(); i++)
	{
		Ghosts[i]->StartNextLoop(StartLocation->GetActorLocation());
	}

	if (CurrentLoopNumber > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		APlayerGhostActor* newGhost =
			GetWorld()->SpawnActor<APlayerGhostActor>(GhostBPClass, StartLocation->GetActorLocation(),
				StartLocation->GetActorRotation(), SpawnParams);
		newGhost->SetFollowLoopNumber(CurrentLoopNumber - 1);
		newGhost->RestartThisLoop(StartLocation->GetActorLocation());
	
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Added new ghost"));
		}
	
		Ghosts.Add(newGhost);
	}

	// reset coins/items? (tbd)

	OnStartNextLoopBP();
}

void AMyGameModeBase::RestartThisLoop()
{
	if (bHasInitializedRace)
	{
		AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		if (player)
		{
			player->StopMovement();
		}
		player->SetActorLocation(StartLocation->GetActorLocation());
		player->SetActorRotation(StartLocation->GetActorRotation());
		
		for (int32 i = 0; i < Ghosts.Num(); i++)
		{
			Ghosts[i]->RestartThisLoop(StartLocation->GetActorLocation());
		}
		
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

		StartNextLoop();
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
