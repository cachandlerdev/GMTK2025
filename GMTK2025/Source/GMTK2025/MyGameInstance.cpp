// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameInstance.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void UMyGameInstance::Init()
{
	Super::Init();
}

void UMyGameInstance::SetMusicVolume(float Volume)
{
	MusicVolume = FMath::Clamp(Volume, 0.1f, 10.0f);
	if (CurrentAudioComponent != nullptr)
	{
		CurrentAudioComponent->AdjustVolume(1, MusicVolume);
	}
}

void UMyGameInstance::PlayMainMenuMusic()
{
	PlayMusicTrack(MainMenuMusicTrack);
}

void UMyGameInstance::PlayMusic()
{
	if (CurrentAudioComponent == nullptr || !CurrentAudioComponent->IsPlaying())
	{
		GoToNextMusicTrack();
	}
	else
	{
		CurrentAudioComponent->SetPaused(false);
	}
}

void UMyGameInstance::PlayMusicTrack(USoundBase* Track)
{
	if (Track == nullptr || GetWorld() == nullptr)
	{
		return;
	}
	if (CurrentAudioComponent != nullptr)
	{
		CurrentAudioComponent->Stop();
		CurrentAudioComponent->OnAudioFinished.Remove(TrackFinished);
	}
	
	float volume = MusicVolume * Track->GetVolumeMultiplier();
	CurrentAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), Track,
		volume, 1, 0.0, nullptr, true);
	TrackFinished.BindUFunction(this, "GoToNextMusicTrack");
	
	if (CurrentAudioComponent == nullptr)
	{
		return;
	}
	
	CurrentAudioComponent->bIsUISound = true;
	CurrentAudioComponent->Activate();
	CurrentAudioComponent->Play(0.0);
}

void UMyGameInstance::PauseMusic()
{
	if (CurrentAudioComponent == nullptr)
	{
		return;
	}

	CurrentAudioComponent->SetPaused(true);
}

void UMyGameInstance::GoToNextMusicTrack()
{
	PlayMusicTrack(GetNextMusicTrack());
}

USoundBase* UMyGameInstance::GetNextMusicTrack()
{
	if (InGameMusicTracks.Num() == 0)
	{
		return nullptr;
	}
	
	int newTrackIndex = FMath::RandRange(0, InGameMusicTracks.Num() - 1);
	return InGameMusicTracks[newTrackIndex];
}

void UMyGameInstance::InitNewLoopData()
{
	// Add the data arrays to track this loop
	FInnerFloatArray speedThisLoop;
	FInnerFloatArray steeringThisLoop;
	FInnerBoolArray wantsToGoForwardOrBackwardsThisLoop;
	FInnerSteerDirectionArray steerDirectionThisLoop;
	FInnerTransformArray transformsThisLoop;
	
	PlayerSpeed.Add(speedThisLoop);
	PlayerSteering.Add(steeringThisLoop);
	PlayerWantsToGoForwardOrBackwards.Add(wantsToGoForwardOrBackwardsThisLoop);
	PlayerSteerDirections.Add(steerDirectionThisLoop);
	
	PlayerTransforms.Add(transformsThisLoop);
}

void UMyGameInstance::ClearPlayerMovementData()
{
	PlayerTransforms.Empty();
	PlayerSpeed.Empty();
	PlayerSteering.Empty();
	PlayerWantsToGoForwardOrBackwards.Empty();
	PlayerSteerDirections.Empty();
}
