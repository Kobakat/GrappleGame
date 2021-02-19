// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleComponent.h"

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	EndLocation = GetAttachedLocation() - GetComponentLocation();
	UCableComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

UGrappleComponent::UGrappleComponent()
{
	// TODO these should not be hardcoded.
	reelMultiplier = 10.F;
	maxGrappleLength = 2500.F;

	PrimaryComponentTick.bCanEverTick = true;
}

FVector UGrappleComponent::GetAttachedLocation()
{
	if (attachedActor != nullptr)
		return attachedLocation + attachedActor->GetActorLocation();
	else
		return FVector::ZeroVector;
}

float UGrappleComponent::GetCableLength()
{
	return currentCableLength;
}

void UGrappleComponent::ApplyForce(FVector force)
{
	// TODO add code that apply's force to current
	// grapple reactor if available.
	if (IsValid(grappleReactor))
	{
		grappleReactor->ApplyPullForce(force);
	}
}

void UGrappleComponent::Reel(float value)
{

	if (currentCableLength + value * reelMultiplier >= 0.F && currentCableLength + value < maxGrappleLength)
	{
		currentCableLength += value * reelMultiplier;
		CableLength = currentCableLength * 0.7F;
	}
	
}

void UGrappleComponent::Attach(FVector vector, AActor* actor)
{
	attachedLocation = vector - actor->GetActorLocation();
	attachedActor = actor;

	bAttachStart = true;

	// gets the distance between the attach point and the start of the cable
	currentCableLength = FMath::Min(FVector::Distance(vector, GetComponentLocation()), maxGrappleLength);
	CableLength = currentCableLength * 0.7F;
}

void UGrappleComponent::Detach()
{
	//bAttachStart = false;
}