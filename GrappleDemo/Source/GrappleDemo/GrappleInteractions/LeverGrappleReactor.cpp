// Fill out your copyright notice in the Description page of Project Settings.


#include "LeverGrappleReactor.h"
#include "DrawDebugHelpers.h"
#include "Math/Vector.h"

bool ALeverGrappleReactor::GetIsSwitched()
{
	return isSwitched;
}

void ALeverGrappleReactor::ApplyPullForce(const FVector force)
{
	// Get the current direction the grapple must pull in to flip
	// the switch state.
	FVector pullDirection = LeverOnDirection;
	if (isSwitched)
		pullDirection = -pullDirection;
	// Get the signed length of the force relative to the switch
	float forceVectorLength = FVector::PointPlaneDist(
		force,
		FVector::ZeroVector,
		pullDirection
	);
	// If there is enough force applied (along the lever normal)
	// then flip the switch
	if (forceVectorLength > ForceThreshold)
	{
		isSwitched = !isSwitched;
		ReceiveOnSwitched(isSwitched);
	}
}
 
