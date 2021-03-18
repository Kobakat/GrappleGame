// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Player/PlayerPawn.h"
#include "CheckpointManager.generated.h"

class ACheckpoint;

UCLASS()
class GRAPPLEDEMO_API ACheckpointManager : public AActor
{
	GENERATED_BODY()

protected:
	// reference of the player
	APawn* player;


	// Array of all checkpoints in the level
	UPROPERTY(EditAnywhere, Category="Checkpoints")
	TArray<ACheckpoint*> Checkpoints;

public:
	// Last checkpoint the player went through
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoints")
	ACheckpoint* currentCheckpoint;

	// Time since player has left start checkpoint
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level Timer")
	float timeElasped;


	UFUNCTION()
	void SetCurrentCheckpoint(ACheckpoint* checkpoint);

	UFUNCTION(BlueprintCallable)
	void CheckLevelStart(ACheckpoint* checkpoint);

	void CheckLevelEnd();
	
protected:
	float startTime;
	bool bTimer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Sets default values for this actor's properties
	ACheckpointManager();
};
