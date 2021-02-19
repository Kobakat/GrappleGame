// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleRaycastComponent.h"
#include "DrawDebugHelpers.h"

UGrappleRaycastComponent::UGrappleRaycastComponent()
{
	reelMultiplier = 10;
	maxGrappleLength = 500;
}

void UGrappleRaycastComponent::Reel(float value)
{
	if (currentCableLength + value * reelMultiplier >= 0 && currentCableLength + value < maxGrappleLength)
	{
		currentCableLength += value * reelMultiplier;
		CableLength += value * reelMultiplier;
	}

}

void UGrappleRaycastComponent::Attach(FVector vector)
{
	bAttachStart = true;
	EndLocation = vector;

	// gets the distance between the attach point and the start of the cable
	currentCableLength = FVector::Distance(vector, RelativeLocation);
}

void UGrappleRaycastComponent::Detach()
{
	bAttachStart = false;
}