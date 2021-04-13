// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollectionMissionData.generated.h"

USTRUCT(BlueprintType)
struct FCollectionMissionData
{
	GENERATED_BODY()
	
	// True if all collectibles have been found.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	bool AllAreCollected;
	// The collected state of each collectible.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	TArray<bool> CollectedStates;
};
