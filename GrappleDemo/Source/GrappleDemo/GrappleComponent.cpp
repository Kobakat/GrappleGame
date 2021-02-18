// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	EndLocation = attachedLocation - GetComponentLocation();
	UCableComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UGrappleComponent::UGrappleComponent()
{
	// TODO these should not be hardcoded.
	reelMultiplier = 10.F;
	maxGrappleLength = 500.F;

	PrimaryComponentTick.bCanEverTick = true;
}

FVector UGrappleComponent::GetAttachedLocation()
{
	return attachedLocation;
}

float UGrappleComponent::GetCableLength()
{
	return currentCableLength;
}

void UGrappleComponent::ApplyForce(FVector force)
{
	// TODO add code that apply's force to current
	// grapple reactor if available.
}

void UGrappleComponent::Reel(float value)
{

	if (currentCableLength + value * reelMultiplier >= 0.F && currentCableLength + value < maxGrappleLength)
	{
		currentCableLength += value * reelMultiplier;
		CableLength = currentCableLength;
	}
	
}

void UGrappleComponent::Attach(FVector vector)
{
	attachedLocation = vector;

	bAttachStart = true;

	// gets the distance between the attach point and the start of the cable
	currentCableLength = FMath::Min(FVector::Distance(vector, RelativeLocation), maxGrappleLength);
}

void UGrappleComponent::Detach()
{
	bAttachStart = false;
}