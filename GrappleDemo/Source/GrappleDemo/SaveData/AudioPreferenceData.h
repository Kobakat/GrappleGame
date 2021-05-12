// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioPreferenceData.generated.h"

USTRUCT(BlueprintType)
struct FAudioPreferenceData
{
	GENERATED_BODY()

	// Expose fields for the distictions of audio in the game.
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Master;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float GrappleSounds;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float VoiceSounds;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float AmbientSounds;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float BackgroundMusic;
};
