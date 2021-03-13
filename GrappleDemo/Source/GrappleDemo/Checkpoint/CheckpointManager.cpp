// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointManager.h"
#include "Checkpoint.h"

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
	

	this->player = GetWorld()->GetFirstPlayerController()->GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *player->GetName());


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

	/*if (Timer)
	{
		LevelTime++;
	}*/

}

void ACheckpointManager::OutOfBounds()
{
	//TODO kill momentum
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *currentCheckpoint->GetName());
	//player->SetActorLocation(currentCheckpoint->GetActorLocation());
}

void ACheckpointManager::SetCurrentCheckpoint(ACheckpoint* checkpoint)
{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *currentCheckpoint->GetName());
	currentCheckpoint = checkpoint;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *currentCheckpoint->GetName());
	CheckLevelEnd();
}

void ACheckpointManager::CheckLevelStart(ACheckpoint* checkpoint)
{
	
	if (checkpoint == currentCheckpoint)
	{
		Timer = true;
	}
}

void ACheckpointManager::CheckLevelEnd()
{
	if (currentCheckpoint == Checkpoints.Last())
	{
		Timer = false;
	}
}