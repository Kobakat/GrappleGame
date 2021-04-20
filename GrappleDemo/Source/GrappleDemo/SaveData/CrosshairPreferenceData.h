// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrosshairPreferenceData.generated.h"

USTRUCT(BlueprintType)
struct FCrosshairPreferenceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	int Index;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Size;
};
