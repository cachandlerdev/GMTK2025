// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"

#include "MyPlayerController.h"
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
		CurrentNumberOfPlayerFailures = 0;
		GameInstance->ClearPlayerMovementData();
	}
}

void AMyGameModeBase::PlayTutorialLines()
{
	UGameplayStatics::PlaySound2D(GetWorld(), TutorialVoiceLines);
}

void AMyGameModeBase::StartFirstLoopWithCountdown()
{
	// Disable player controls
	// Play countdown timer
	// Display UI stuff (via the blueprint)
	// Have a timer that waits before then calling start next loop

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PlayerController)
	{
		PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			PlayerPawn->DisableInput(PlayerController);
		}
	}
	
	SetupPlayerForLoop();
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FirstLoopSound, PlayerPawn->GetActorLocation(), PlayerPawn->GetActorRotation());
	
	GetWorldTimerManager().SetTimer(FirstLoopCountdownHandle, this, &AMyGameModeBase::StartNextLoop,
		InitialCountdownDuration, false);	
}

void AMyGameModeBase::SetLevelBaselineTime(int32 Seconds)
{
	if (Seconds > 0)
	{
		BestLoopTimeInSeconds = Seconds;
	}
}

void AMyGameModeBase::StartNextLoop()
{
	if (PlayerController && PlayerPawn)
	{
		if (!PlayerPawn->InputEnabled())
		{
			PlayerPawn->EnableInput(PlayerController);
		}
	}
	
	CurrentLoopNumber++;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,FString::Printf(TEXT("Start loop %i"), CurrentLoopNumber));
	}

	SetupPlayerForLoop();
	//GetWorldTimerManager().SetTimer(SlowTimeHandle, this, &AMyGameModeBase::SetupPlayerForLoop,
	//	DelayTimePerLoopForPlayer, false);	

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
		
		UGameplayStatics::PlaySound2D(GetWorld(), NewLoopSound);
	}
	else
	{
		UGameplayStatics::PlaySound2D(GetWorld(), NoRecordedTimesVoiceLine);
	}

	CurrentLoopStartTime = GetWorld()->TimeSeconds;

	// reset coins/items? (tbd)
	LoopFinishedDelegateCalls();
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
		player->CameraBoom->bEnableCameraLag = false;
		player->CameraBoom->bEnableCameraRotationLag = false;
		player->SetActorLocation(StartLocation->GetActorLocation());
		player->SetActorRotation(StartLocation->GetActorRotation());
		player->CameraBoom->bEnableCameraLag = true;
		player->CameraBoom->bEnableCameraRotationLag = true;
		
		for (int32 i = 0; i < Ghosts.Num(); i++)
		{
			Ghosts[i]->RestartThisLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
		}

		GameInstance->PlayerSpeed[CurrentLoopNumber].ArrayOfFloats.Reset();
		GameInstance->PlayerSteering[CurrentLoopNumber].ArrayOfFloats.Reset();
		GameInstance->PlayerWantsToGoForwardOrBackwards[CurrentLoopNumber].ArrayOfBools.Reset();
		GameInstance->PlayerSteerDirections[CurrentLoopNumber].ArrayOfDirections.Reset();
		
		CurrentLoopStartTime = GetWorld()->TimeSeconds;
		
		UGameplayStatics::PlaySound2D(GetWorld(), RestartLoopSound);
		LoopFinishedDelegateCalls();
		OnRestartThisLoopBP();
	}
}

void AMyGameModeBase::FinishThisLoop()
{
	if (bHasInitializedRace)
	{
		int32 playerTime = GetCurrentLoopTimeInSeconds();
		if (playerTime > BestLoopTimeInSeconds)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Player lost this round")
				);
			}
			
			// Player loses one "heart"/"chance"
			CurrentNumberOfPlayerFailures++;
			if (CurrentNumberOfPlayerFailures >= NumberOfPlayerFailuresTolerated)
			{
				// Player loses game
				OnLoseGame();
				return;	
			}
			
			// Player keeps going
			OnLoseRoundBP();
			UGameplayStatics::PlaySound2D(GetWorld(), RoundLostVoiceLine);
		}
		else
		{
			// Player won again
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Red,TEXT("Player won this round")
				);
			}
			CurrentNumberOfPlayerFailures = 0; // We count consecutive failures
			BestLoopTimeInSeconds = playerTime;
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

void AMyGameModeBase::OnLoseGame()
{
	// todo

	UGameplayStatics::PlaySound2D(GetWorld(), GameLoseVoiceLine);

	bHasInitializedRace = false;
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.0f, FColor::Yellow,TEXT("Player lost. End of race."));
	}
	OnLoseGameBP();
}
void AMyGameModeBase::LoopFinishedDelegateCalls()
{
	OnFinishedLapDelegate.Broadcast();
}