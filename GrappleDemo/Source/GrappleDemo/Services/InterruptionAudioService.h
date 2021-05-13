// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Audio/InterruptionSystemAudioTrigger.h"
#include "../GrappleGameInstance.h"
#include "InterruptionAudioService.generated.h"

UCLASS()
class GRAPPLEDEMO_API UInterruptionAudioService : public UObject
{
	GENERATED_BODY()

public:
	UInterruptionAudioService();
	~UInterruptionAudioService();
	// Gets the singleton audio service.
	static UInterruptionAudioService* GetInstance(AActor* requester);

	// Clears all audio requests from the queue.
	void ClearQueue();
	// Requests that a trigger's audio effect be played.
	void RequestPlay(AInterruptionSystemAudioTrigger* trigger);
	// Notifies that a trigger no longer wants to be played,
	// or has completed playing.
	void RetractRequest(AInterruptionSystemAudioTrigger* trigger);

private:
	static UInterruptionAudioService* instance;
	// Stores the current requests from audio trigger sources.
	TArray<AInterruptionSystemAudioTrigger*> Requests;
	AInterruptionSystemAudioTrigger* CurrentlyPlayingAudio;
	// Stores a reference to the game instance to retrieve preferences.
	UGrappleGameInstance* GameInstance;
};
