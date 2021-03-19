// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverGrappleReactor.h"
#include "DrawDebugHelpers.h"
#include "Math/Vector.h"

void ALeverGrappleReactor::BeginPlay()
{
	dotThreshold = FMath::Cos(FMath::DegreesToRadians(AngleTolerance));

	//UE_LOG(LogTemp, Warning, TEXT("DOT: %f"), dotThreshold);
}

bool ALeverGrappleReactor::GetIsSwitched()
{
	return isSwitched;
}

void ALeverGrappleReactor::ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance)
{
	// Get the current direction the grapple must pull in to flip
	// the switch state.
	FVector pullDirection = GetActorForwardVector();
	if (isSwitched)
		pullDirection = -pullDirection;

	float dotProduct = FVector::DotProduct((pullTowards - pullPoint).GetSafeNormal(), pullDirection.GetSafeNormal());

	// If there is enough force applied (along the lever normal)
	// then flip the switch
	if (dotProduct > dotThreshold)
	{
		isSwitched = !isSwitched;
		ReceiveOnSwitched(isSwitched);
	}
}
 
