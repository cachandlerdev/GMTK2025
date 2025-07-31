// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RaceEndLocation.h"
#include "RaceStartLocation.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

// To avoid a circular dependency
/**
 * 
 */
UCLASS()
class GMTK2025_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category="Loop")
	int32 NumOfLoops = 5;

protected:

	// There can only be one start location per level.
	UPROPERTY(BlueprintReadOnly, Category="Loop")
	ARaceStartLocation* StartLocation;
	
	// There can only be one end location per level.
	UPROPERTY(BlueprintReadOnly, Category="Loop")
	ARaceEndLocation* EndLocation;

private:

	bool bHasInitializedRace = false;
	int32 CurrentLoopNumber = 0;

public:
	
	UFUNCTION(BlueprintCallable, Category="Loop")
	void InitRaceLogic();

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

private:
	bool CanInitRaceLogic(TArray<AActor*> startActors, TArray<AActor*> endActors);
	
};
