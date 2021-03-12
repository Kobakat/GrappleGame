// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Checkpoint.h"
#include "../Player/PlayerPawn.h"
#include "CheckpointManager.generated.h"

UCLASS()
class GRAPPLEDEMO_API ACheckpointManager : public AActor
{
	GENERATED_BODY()

	// TODO----------------------------
		// Start timer when player leaves first checkpoint
		// End Timer when player hits last checkpoint


protected:
	// reference of the player
	APlayerPawn player;

private:
	// Array of all checkpoints in the level
	UPROPERTY(EditAnywhere, Category="Checkpoints")
	TArray<ACheckpoint*> Checkpoints;

	// Last checkpoint the player went through
	UPROPERTY(VisibleAnywhere)
	static ACheckpoint* currentCheckpoint;

	UPROPERTY(BlueprintReadWrite)
	double LevelTime;

	bool Timer;

	UFUNCTION(BlueprintCallable)
	void OutOfBounds();
	
public:	
	// Sets default values for this actor's properties
	ACheckpointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
