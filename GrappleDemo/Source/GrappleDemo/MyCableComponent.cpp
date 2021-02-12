// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCableComponent.h"

UMyCableComponent::UMyCableComponent()
{
	reelMultiplier = 10;
	maxGrappleLength = 500;
}

void UMyCableComponent::Reel(float value)
{
	if (CableLength + value * reelMultiplier >= 0 && CableLength + value < maxGrappleLength)
	{
		CableLength += value * reelMultiplier;
	}
	
}

void UMyCableComponent::Attach(FVector vector)
{
	bAttachStart = true;
	EndLocation = vector;
}

void UMyCableComponent::Detach()
{
	bAttachStart = false;
}