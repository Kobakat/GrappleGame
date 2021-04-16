// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraPreferenceData.generated.h"

USTRUCT(BlueprintType)
struct FCameraPreferenceData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float FieldOfView;
	UPROPERTY(BlueprintReadWrite, Category = "PreferenceData")
	float Sensitivity;
};
