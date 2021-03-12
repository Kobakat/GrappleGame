// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	APawn* player;

public:
	// Array of all checkpoints in the level
	UPROPERTY(EditAnywhere, Category="Checkpoints")
	TArray<AActor*> Checkpoints;

	// Last checkpoint the player went through
	UPROPERTY(VisibleAnywhere)
	AActor* currentCheckpoint;

	UPROPERTY(BlueprintReadWrite)
	float LevelTime;

	bool Timer;

	UFUNCTION(BlueprintCallable)
	void OutOfBounds();

	UFUNCTION()
	void SetCurrentCheckpoint(AActor* checkpoint);

	UFUNCTION(BlueprintCallable)
	void CheckLevelStart(AActor* checkpoint);

	void CheckLevelEnd();
	

	
	// Sets default values for this actor's properties
	ACheckpointManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
