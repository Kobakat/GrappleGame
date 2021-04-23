// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LevelCompletionData.h"
#include "LevelCompletionSave.generated.h"

UCLASS()
class GRAPPLEDEMO_API ULevelCompletionSave : public USaveGame
{
	GENERATED_BODY()

public:
	// The underlying save data for the level.
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	FLevelCompletionData CompletionData;
};
