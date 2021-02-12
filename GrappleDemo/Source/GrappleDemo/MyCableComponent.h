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

	UMyCableComponent();


private:
	float currentCableLength;
	
public:

	UPROPERTY(EditAnywhere)
	float reelMultiplier;
	UPROPERTY(EditAnywhere)
	float maxGrappleLength;

	void Reel(float value);

	// function that takes in vector and attaches endpoint of grapple to that
	void Attach(FVector vector);

	void Detach();
	
};
