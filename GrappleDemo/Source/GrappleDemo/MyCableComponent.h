// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "MyCableComponent.generated.h"

/**
 * 
 */
UCLASS()
class GRAPPLEDEMO_API UMyCableComponent : public UCableComponent
{
	GENERATED_BODY()
	
public:

	void Reel(float value);

};
