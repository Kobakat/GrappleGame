// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollectionMissionData.h"
#include "LevelCompletionData.h"
#include "LevelProgress.generated.h"

// This struct is used to communicate with blueprints.
// It is assembled and disassembled into save slots to
// allow for the save slots to be more flexible.
USTRUCT(BlueprintType)
struct FLevelProgress
{
	GENERATED_BODY()

	// The basic stage data assigned to all stages.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	FLevelCompletionData CompletionData;
	// The basic stage data assigned to all stages.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	TArray<FCollectionMissionData> CollectionMissions;
};
