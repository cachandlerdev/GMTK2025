// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyEnums.h"
#include "MyStructs.h"
#include "MyGameInstance.generated.h"

/**
 * The game instance for the project. This class keeps track of the game music.
 */
UCLASS()
class GMTK2025_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	// The volume of the music tracks.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Music")
	float MusicVolume = 1.0;

	// The track that plays in the main menu
	UPROPERTY(EditAnywhere, Category="Music")
	USoundBase* MainMenuMusicTrack;

	// The list of music tracks.
	UPROPERTY(EditAnywhere, Category="Music")
	TArray<USoundBase*> InGameMusicTracks;

	void Init() override;

	// Updates the music volume
	UFUNCTION(BlueprintCallable, Category="Music")
	void SetMusicVolume(float Volume);

	// Play the main menu music track
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayMainMenuMusic();

	// Starts playing music
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayMusic();

	// Play a specific music track
	UFUNCTION(BlueprintCallable, Category="Music")
	void PlayMusicTrack(USoundBase* Track);

	// Pauses the music
	UFUNCTION(BlueprintCallable, Category="Music")
	void PauseMusic();

	// Go to the next music track
	UFUNCTION(BlueprintCallable, Category="Music")
	void GoToNextMusicTrack();

	// Gets the next music track to be played (currently grabs a random track)
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Music")
	USoundBase* GetNextMusicTrack();

	// Ghost logic

	// OLD
	//Stores the player position over time
	UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	TArray<FTransform> PlayerPositions;

	// NEW
	
	//UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	//TArray<float> PlayerSpeed;
	TArray<FInnerFloatArray> PlayerSpeed;
	
	//UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	//TArray<float> PlayerSteering;
	TArray<FInnerFloatArray> PlayerSteering;
	
	//UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	//TArray<bool> PlayerWantsToGoForwardOrBackwards;
	TArray<FInnerBoolArray> PlayerWantsToGoForwardOrBackwards;
	
	//UPROPERTY(BlueprintReadOnly, Category = "Ghost")
	//TArray<ESteerDirection> PlayerSteerDirections;
	TArray<FInnerSteerDirectionArray> PlayerSteerDirections;

private:
	UPROPERTY()
	USoundBase* CurrentMusicTrack;

	UPROPERTY()
	UAudioComponent* CurrentAudioComponent;

	// Used to start the next track after this one finishes.
	TScriptDelegate<FWeakObjectPtr> TrackFinished;
};
