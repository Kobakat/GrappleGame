// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CollectionMissionData.h"
#include "LevelCompletionData.generated.h"

USTRUCT(BlueprintType)
struct FLevelCompletionData
{
	GENERATED_BODY()

	// Whether the stage has been completed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	bool StageCompleted;
	// The best time achieved on this stage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MissionData")
	float BestTime;
};
