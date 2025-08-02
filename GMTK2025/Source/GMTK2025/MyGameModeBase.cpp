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
	
	GetWorldTimerManager().SetTimer(SlowTimeHandle, this, &AMyGameModeBase::SetupPlayerForLoop,
		DelayTimePerLoopForPlayer, false);	
	

	// Add the data arrays to track this loop
	GameInstance->InitNewLoopData();

	for (int32 i = 0; i < Ghosts.Num(); i++)
	{
		Ghosts[i]->StartNextLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
	}

	if (CurrentLoopNumber > 0)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		APlayerGhostActor* newGhost =
			GetWorld()->SpawnActor<APlayerGhostActor>(GhostBPClass, FVector(0, 0, 0),
				FRotator(0, 0, 0), SpawnParams);
		newGhost->SetFollowLoopNumber(CurrentLoopNumber - 1);
		newGhost->RestartThisLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
	
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Added new ghost"));
		}
	
		Ghosts.Add(newGhost);
	}

	CurrentLoopStartTime = GetWorld()->TimeSeconds;

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
			Ghosts[i]->RestartThisLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
		}

		GameInstance->PlayerSpeed[CurrentLoopNumber].ArrayOfFloats.Reset();
		GameInstance->PlayerSteering[CurrentLoopNumber].ArrayOfFloats.Reset();
		GameInstance->PlayerWantsToGoForwardOrBackwards[CurrentLoopNumber].ArrayOfBools.Reset();
		GameInstance->PlayerSteerDirections[CurrentLoopNumber].ArrayOfDirections.Reset();
		
		CurrentLoopStartTime = GetWorld()->TimeSeconds;
		
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

int32 AMyGameModeBase::GetCurrentLoopTimeInSeconds()
{
	return GetWorld()->TimeSeconds - CurrentLoopStartTime;
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
			GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Error: Couldn't find race end location.")
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

void AMyGameModeBase::SetupPlayerForLoop()
{
	AHoverVehiclePawn* player = Cast<AHoverVehiclePawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (player)
	{
		player->StopMovement();
		if (StartLocation)
		{
			player->SetActorLocation(StartLocation->GetActorLocation());
			player->SetActorRotation(StartLocation->GetActorRotation());
		}
	}
}
