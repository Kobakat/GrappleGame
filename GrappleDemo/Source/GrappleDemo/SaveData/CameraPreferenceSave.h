// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CameraPreferenceData.h"
#include "CameraPreferenceSave.generated.h"

UCLASS()
class GRAPPLEDEMO_API UCameraPreferenceSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	// The underlying save data for camera preferences.
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	FCameraPreferenceData CameraPreferences;
};
