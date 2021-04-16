// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CrosshairPreferenceData.h"
#include "CrosshairPreferenceSave.generated.h"

UCLASS()
class GRAPPLEDEMO_API UCrosshairPreferenceSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	// The underlying save data for crosshair preferences.
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	FCrosshairPreferenceData CrosshairPreferences;
};
