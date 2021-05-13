// Fill out your copyright notice in the Description page of Project Settings.


#include "InterruptionSystemAudioTrigger.h"
#include "../Services/InterruptionAudioService.h"

// Sets default values
AInterruptionSystemAudioTrigger::AInterruptionSystemAudioTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
}

bool AInterruptionSystemAudioTrigger::GetWillInterrupt()
{
	return WillInterrupt;
}

// Called when the game starts or when spawned
void AInterruptionSystemAudioTrigger::BeginPlay()
{
	Super::BeginPlay();
	// Immediately clear queue on begin play.
	// TODO this is an un-organized way to clear
	// this state; should find a better way to watch
	// audio requests being cut off by loads.
	UInterruptionAudioService::GetInstance(this)->ClearQueue();
}

// Wraps the singleton, these are invoked by the blueprint.

void AInterruptionSystemAudioTrigger::RequestPlay()
{
	UInterruptionAudioService::GetInstance(this)->RequestPlay(this);
}
void AInterruptionSystemAudioTrigger::RemoveRequest()
{
	UInterruptionAudioService::GetInstance(this)->RetractRequest(this);
}
