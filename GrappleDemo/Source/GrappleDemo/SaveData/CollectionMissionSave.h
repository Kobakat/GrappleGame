// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CollectionMissionData.h"
#include "CollectionMissionSave.generated.h"

UCLASS()
class GRAPPLEDEMO_API UCollectionMissionSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	// The collection mission data.
	UPROPERTY(BlueprintReadWrite, Category = "MissionData")
	FCollectionMissionData CollectionMissionData;
};
