// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyGameInstance.h"
#include "PlayerGhostActor.h"
#include "RaceEndLocation.h"
#include "RaceStartLocation.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedLapDelegate);
// To avoid a circular dependency
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishedLapDelegate);
UCLASS()
class GMTK2025_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Loop")
	int32 NumOfLoops = 5;
	
	UPROPERTY(EditAnywhere, Category="Loop")
	TSubclassOf<APlayerGhostActor> GhostBPClass;

	UPROPERTY(EditAnywhere, Category="Loop")
	float DelayTimePerLoopForPlayer = 0.2f;

	UPROPERTY(BlueprintReadOnly, Category="Loop")
	int32 BestLoopTimeInSeconds = TNumericLimits<int32>::Max();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Loop")
	int32 NumberOfPlayerFailuresTolerated = 3;

	UPROPERTY(BlueprintReadOnly, Category="Loop")
	int32 CurrentNumberOfPlayerFailures = 0;

	UPROPERTY(BlueprintReadOnly, Category="Loop")
	float InitialCountdownDuration = 2.22f;

	// Sound
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* FirstLoopSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* NewLoopSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* RestartLoopSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* GameLoseSound;

	// Voice lines
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* TutorialVoiceLines;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* SystemInitializingVoiceLine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* NoRecordedTimesVoiceLine;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* TimeUpdatedVoiceLine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* RoundLostVoiceLine;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* GameLoseVoiceLine;

protected:

	// There can only be one start location per level.
	UPROPERTY(BlueprintReadOnly, Category="Loop")
	ARaceStartLocation* StartLocation;
	
	// There can only be one end location per level.
	UPROPERTY(BlueprintReadOnly, Category="Loop")
	ARaceEndLocation* EndLocation;

private:

	bool bHasInitializedRace = false;
	// The first "active recording" loop number is 0
	int32 CurrentLoopNumber = -1;
	
	TArray<APlayerGhostActor*> Ghosts;

	UMyGameInstance* GameInstance;
	
	int32 CurrentLoopTimer = 0;
	int32 CurrentLoopStartTime = 0;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFinishedLapDelegate OnFinishedLapDelegate;

	// Call this when you want to trigger pickup respawn
	UFUNCTION(BlueprintCallable, Category = "Loop")
	void LoopFinishedDelegateCalls();
	
	// Used for slowing the player each loop
	FTimerHandle SlowTimeHandle;
	// Used for the initial countdown
	FTimerHandle FirstLoopCountdownHandle;
	APlayerController* PlayerController;
	APawn* PlayerPawn;

	// Used to play the lose sound effect.
	FTimerHandle GameLoseSoundDelayHandle;
public:
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void InitRaceLogic();
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void PlayTutorialLines();
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void StartFirstLoopWithCountdown();
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void SetLevelBaselineTime(int32 Seconds);

	UFUNCTION(BlueprintCallable, Category="Loop")
	void StartNextLoop();
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void RestartThisLoop();
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void FinishThisLoop();

	UFUNCTION(BlueprintCallable, Category="Loop")
	int32 GetCurrentLoopNumber();

	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnStartNextLoopBP();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnRestartThisLoopBP();

	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnFinishThisLoopBP();
	
	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnLoseRoundBP();

	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnLoseGameBP();

	UFUNCTION(BlueprintImplementableEvent, Category="Loop")
	void OnResetCurrentNumOfFailuresBP();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Loop")
	int32 GetCurrentLoopTimeInSeconds();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnFinishedLapDelegate OnFinishedLapDelegate;

	// Call this when you want to trigger pickup respawn
	UFUNCTION(BlueprintCallable, Category = "Loop")
	void LoopFinishedDelegateCalls();

private:
	bool CanInitRaceLogic(TArray<AActor*> startActors, TArray<AActor*> endActors);

	void SetupPlayerForLoop();

	void PlayLoseSound();
	void OnLoseGame();
	void MovePawnToStart(APawn* Pawn);

	void AddNewGhost();
};
