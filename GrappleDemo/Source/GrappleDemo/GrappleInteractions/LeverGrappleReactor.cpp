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

void ALeverGrappleReactor::ApplyPullForce(const FVector force)
{
	// Get the current direction the grapple must pull in to flip
	// the switch state.
	FVector pullDirection = GetActorForwardVector();
	if (isSwitched)
		pullDirection = -pullDirection;

	float dotProduct = FVector::DotProduct(force.GetSafeNormal(), pullDirection.GetSafeNormal());

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Green, FString::SanitizeFloat(dotProduct));
	}

	//UE_LOG(LogTemp, Warning, TEXT("DOT: %f"), dotProduct);

	// If there is enough force applied (along the lever normal)
	// then flip the switch
	if (dotProduct > dotThreshold)
	{
		isSwitched = !isSwitched;
		ReceiveOnSwitched(isSwitched);
	}
}
 
