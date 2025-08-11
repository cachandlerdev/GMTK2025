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
	SetupPlayerForLoop();

	// Add the data arrays to track this loop
	GameInstance->InitNewLoopData();

	for (int32 i = 0; i < Ghosts.Num(); i++)
	{
		Ghosts[i]->StartNextLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
	}

	if (CurrentLoopNumber > 0)
	{
		AddNewGhost();
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
	if (bHasInitializedRace && CurrentLoopNumber >= 0)
	{
		SetupPlayerForLoop();
		
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
			if (CurrentNumberOfPlayerFailures > 0)
			{
				// Player recovered, chances reset
				OnResetCurrentNumOfFailuresBP();
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
			FVector newLocation = StartLocation->GetActorLocation();
			newLocation.Z += player->GetDefaultHalfHeight();
			player->SetActorLocation(newLocation);
			player->SetActorRotation(StartLocation->GetActorRotation());
			player->Chassis->SetWorldLocation(newLocation);
			player->Chassis->SetWorldRotation(StartLocation->GetActorRotation());
		}
	}
}

void AMyGameModeBase::PlayLoseSound()
{
	UGameplayStatics::PlaySound2D(GetWorld(), GameLoseSound);
}

void AMyGameModeBase::OnLoseGame()
{
	// todo

	UGameplayStatics::PlaySound2D(GetWorld(), GameLoseVoiceLine);
	GetWorldTimerManager().SetTimer(GameLoseSoundDelayHandle, this, &AMyGameModeBase::PlayLoseSound,
		GameLoseVoiceLine->GetDuration(), false);	

	bHasInitializedRace = false;
	OnLoseGameBP();
}

void AMyGameModeBase::AddNewGhost()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APlayerGhostActor* newGhost =
		GetWorld()->SpawnActor<APlayerGhostActor>(GhostBPClass, FVector(0, 0, 0),
			FRotator(0, 0, 0), SpawnParams);
	newGhost->SetFollowLoopNumber(CurrentLoopNumber - 1);
	newGhost->RestartThisLoop(StartLocation->GetActorLocation(), StartLocation->GetActorRotation());
	
	Ghosts.Add(newGhost);
}

void AMyGameModeBase::LoopFinishedDelegateCalls()
{
	OnFinishedLapDelegate.Broadcast();
}
