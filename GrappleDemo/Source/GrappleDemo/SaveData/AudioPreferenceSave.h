// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AudioPreferenceData.h"
#include "AudioPreferenceSave.generated.h"

UCLASS()
class GRAPPLEDEMO_API UAudioPreferenceSave : public USaveGame
{
	GENERATED_BODY()
	
public:
	// The underlying save data for audio preferences.
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	FAudioPreferenceData AudioPreferences;
};
