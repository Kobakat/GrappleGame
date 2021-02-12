// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCableComponent.h"

UMyCableComponent::UMyCableComponent()
{
	reelMultiplier = 10;
	maxGrappleLength = 500;
}

void UMyCableComponent::Reel(float value)
{
	if (currentCableLength + value * reelMultiplier >= 0 && currentCableLength + value < maxGrappleLength)
	{
		currentCableLength += value * reelMultiplier;
		CableLength += value * reelMultiplier;
	}
	
}

void UMyCableComponent::Attach(FVector vector)
{
	bAttachStart = true;
	EndLocation = vector;

	// gets the distance between the attach point and the start of the cable
	currentCableLength = FVector::Distance(vector, RelativeLocation);
}

void UMyCableComponent::Detach()
{
	bAttachStart = false;
}