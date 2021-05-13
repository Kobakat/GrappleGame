// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../SaveData/AudioPreferenceData.h"
#include "InterruptionSystemAudioTrigger.generated.h"

UCLASS()
class GRAPPLEDEMO_API AInterruptionSystemAudioTrigger : public AActor
{
	GENERATED_BODY()
	
protected:
	// If true this clip will interrupt other clips.
	UPROPERTY(EditAnywhere, Category = "Audio Service")
	bool WillInterrupt;

public:	
	// Sets default values for this actor's properties
	AInterruptionSystemAudioTrigger();

	// When the audio singleton decides this can play,
	// this is the method that it will call.
	UFUNCTION(BlueprintImplementableEvent)
	void PlaySound(FAudioPreferenceData Preferences);
	// Implements logic to stop playing audio when
	// the audio service decides a new clip should play.
	UFUNCTION(BlueprintImplementableEvent)
	void Interrupt();

	// Gets whether this audio is important and will inerrupt other audio.
	bool GetWillInterrupt();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Requests that the clip be played.
	UFUNCTION(BlueprintCallable, Category = "Audio Service")
	void RequestPlay();
	// Removes the play request for this clip;
	// signaling that it is done or no longer relevant to play.
	UFUNCTION(BlueprintCallable, Category = "Audio Service")
	void RemoveRequest();
};
