// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AudioPreferenceData.generated.h"

USTRUCT(BlueprintType)
struct FAudioPreferenceData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Master;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Sound;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Music;
};
