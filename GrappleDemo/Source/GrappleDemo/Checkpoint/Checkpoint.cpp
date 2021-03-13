// Fill out your copyright notice in the Description page of Project Settings.


#include "Checkpoint.h"

// Sets default values
ACheckpoint::ACheckpoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACheckpoint::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ACheckpoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACheckpoint::SetCheckpoint()
{
	checkpointManager->SetCurrentCheckpoint(this);
}

void ACheckpoint::CheckLevelStart()
{
	checkpointManager->CheckLevelStart(this);
	// Check level end
}

void ACheckpoint::FellOutOfBounds()
{
	checkpointManager->OutOfBounds();
}

void ACheckpoint::SetCheckpointManager(ACheckpointManager* cpManager)
{
	this->checkpointManager = cpManager;
}