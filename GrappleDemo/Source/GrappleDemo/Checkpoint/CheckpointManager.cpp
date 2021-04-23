// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointManager.h"
#include "Engine/World.h"
#include "Checkpoint.h"
#include "Kismet/GameplayStatics.h"
#include "../GrappleGameInstance.h"

// Sets default values
ACheckpointManager::ACheckpointManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckpointManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Sets player pawn
	this->player = GetWorld()->GetFirstPlayerController()->GetPawn();

	// Sets the chechkpoint manager instance for each checkpoint in the array
	for (ACheckpoint* checkpoint : Checkpoints)
	{
		checkpoint->SetCheckpointManager(this);
	}

	// The first checkpoint in the array is the start by default
	currentCheckpoint = Checkpoints[0];
}

// Called every frame
void ACheckpointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//TODO Show in UI
	if (bTimer)
	{
		timeElasped = GetWorld()->GetRealTimeSeconds() - startTime;
		UE_LOG(LogTemp, Warning, TEXT("%f"), timeElasped);
	}
}

void ACheckpointManager::SetCurrentCheckpoint(ACheckpoint* checkpoint)
{
	// Sets the last checkpoint the player has hit
	currentCheckpoint = checkpoint;
	CheckLevelEnd();
}

void ACheckpointManager::CheckLevelStart(ACheckpoint* checkpoint)
{
	if (checkpoint == Checkpoints[0])
	{
		// Sets the game time to when the player leaves the start checkpoint
		startTime = GetWorld()->GetRealTimeSeconds();
		bTimer = true;
	}
}

void ACheckpointManager::ResetGrapple()
{
	APlayerPawn* playerPawn = Cast<APlayerPawn>(player);
	playerPawn->grappleComponent->ResetDetach();
	playerPawn->SetState(UWalkState::GetInstance());
}

void ACheckpointManager::CheckLevelEnd()
{
	// Checks if the player has entered the last checkpoint in the array
	if (currentCheckpoint == Checkpoints.Last())
	{
		bTimer = false;
		// Grab the game instance containing save data.
		UGrappleGameInstance* instance = 
			Cast<UGrappleGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		// Get the level progress.
		FLevelProgress progress = instance->GetLevelProgress(Stage);
		// Update the time record if it is better.
		float time = GetWorld()->GetRealTimeSeconds() - startTime;
		if (time < progress.CompletionData.BestTime)
			progress.CompletionData.BestTime = time;
		// Mark stage as completed.
		progress.CompletionData.StageCompleted = true;
		// Commit progress and save to disk.
		instance->SetLevelProgress(progress, Stage);
		instance->Save();
	}
}