// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "../Player/PlayerPawn.h"
#include "CheckpointManager.generated.h"

class ACheckpoint;

UCLASS()
class GRAPPLEDEMO_API ACheckpointManager : public AActor
{
	GENERATED_BODY()

	// TODO----------------------------
		// Start timer when player leaves first checkpoint
		// End Timer when player hits last checkpoint


protected:
	// reference of the player
	APawn* player;

public:
	// Array of all checkpoints in the level
	UPROPERTY(EditAnywhere, Category="Checkpoints")
	TArray<ACheckpoint*> Checkpoints;

	// Last checkpoint the player went through
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ACheckpoint* currentCheckpoint;

	UPROPERTY(BlueprintReadWrite)
	float LevelTime;

	bool Timer;

	UFUNCTION(BlueprintCallable)
	void OutOfBounds();

	UFUNCTION()
	void SetCurrentCheckpoint(ACheckpoint* checkpoint);

	UFUNCTION(BlueprintCallable)
	void CheckLevelStart(ACheckpoint* checkpoint);

	void CheckLevelEnd();
	
	float TimeElasped;
	
	// Sets default values for this actor's properties
	ACheckpointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
