// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableActor.h"
#include "MyCableActor.generated.h"

/**
 * 
 */
UCLASS()
class GRAPPLEDEMO_API AMyCableActor : public ACableActor
{
	GENERATED_BODY()
	

public:

	void Attach();

	void Reel();
};
