// Fill out your copyright notice in the Description page of Project Settings.


#include "InterruptionAudioService.h"
#include "Kismet/GameplayStatics.h"

UInterruptionAudioService::UInterruptionAudioService() { }
UInterruptionAudioService::~UInterruptionAudioService() { }

UInterruptionAudioService* UInterruptionAudioService::instance;
UInterruptionAudioService* UInterruptionAudioService::GetInstance(AActor* requester)
{
	// Lazily initialize the singleton.
	if (!IsValid(instance))
	{
		instance = NewObject<UInterruptionAudioService>();
		instance->GameInstance = Cast<UGrappleGameInstance>(
			UGameplayStatics::GetGameInstance(requester));
		instance->AddToRoot();
	}
	return instance;
}

void UInterruptionAudioService::ClearQueue()
{
	Requests.SetNum(0);
	CurrentlyPlayingAudio = nullptr;
	GameInstance->ClearAudioRuntimeMultiplier();
}

void UInterruptionAudioService::RequestPlay(AInterruptionSystemAudioTrigger* trigger)
{
	// This clip can play now if it is interrupting,
	// or if there are no other requests in the queue.
	if (trigger->GetWillInterrupt() ||
		(Requests.Num() == 0 && !IsValid(CurrentlyPlayingAudio)))
	{
		// If there is clip playing, interrupt it.
		if (IsValid(CurrentlyPlayingAudio))
			CurrentlyPlayingAudio->Interrupt();
		// Clear other clips in the queue.
		if (Requests.Num() > 0)
			ClearQueue();
		// Play the new clip.
		trigger->PlaySound(GameInstance->GetAudioPreferences());
		CurrentlyPlayingAudio = trigger;
		// TODO make this more DRY; reduces other noise while dialogue plays.
		// TODO The 50% here NEEDS to be exposed to designers. 
		FAudioPreferenceData modifier;
		modifier.AmbientSounds =
			modifier.GrappleSounds =
			modifier.BackgroundMusic = 0.5F;
		modifier.Master =
			modifier.VoiceSounds = 1.F;
		GameInstance->SetAudioRuntimeMultiplier(modifier);
	}
	else
	{
		// Place the clip at the back of the queue.
		Requests.Add(trigger);
	}
}

void UInterruptionAudioService::RetractRequest(AInterruptionSystemAudioTrigger* trigger)
{
	// Is this the current clip playing?
	if (trigger == CurrentlyPlayingAudio)
	{
		// Are there more clips that want to play?
		if (Requests.Num() > 0)
		{
			// Play the next clip.
			Requests[0]->PlaySound(GameInstance->GetAudioPreferences());
			CurrentlyPlayingAudio = Requests[0];
			// Shift the requests along.
			Requests.RemoveAt(0);
		}
		else
		{
			// Clear state of currently playing audio.
			CurrentlyPlayingAudio = nullptr;
			// TODO make this more DRY; restores audio volume.
			GameInstance->ClearAudioRuntimeMultiplier();
		}
	}
	else
	{
		// Simply remove the request from the queued requests.
		Requests.Remove(trigger);
	}
}
