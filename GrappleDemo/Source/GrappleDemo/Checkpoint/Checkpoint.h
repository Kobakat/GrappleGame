// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointManager.h"
#include "Checkpoint.generated.h"

UCLASS()
class GRAPPLEDEMO_API ACheckpoint : public AActor
{
	GENERATED_BODY()


	ACheckpointManager* checkpointManager;
	
public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	UPROPERTY(EditAnywhere, Category = "UI Text")
	FText UIText;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetCheckpoint();

	UFUNCTION(BlueprintCallable)
	void CheckLevelStart();

	void SetCheckpointManager(ACheckpointManager* cpManager);
};
