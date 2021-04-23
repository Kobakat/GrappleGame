// Fill out your copyright notice in the Description page of Project Settings.


#include "TorquePullableGrappleReactor.h"
#include "DrawDebugHelpers.h"

ATorquePullableGrappleReactor::ATorquePullableGrappleReactor()
{

}

void ATorquePullableGrappleReactor::BeginPlay()
{

}

void ATorquePullableGrappleReactor::ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance)
{
	// Get the pivot and rotation vector.
	FVector actorLocation = GetActorLocation();
	FVector axisNormal;
	switch (Axis)
	{
	case EAxis::X:
		axisNormal = FVector::ForwardVector;
		break;
	case EAxis::Y:
		axisNormal = FVector::RightVector;
		break;
	case EAxis::Z:
		axisNormal = FVector::UpVector;
		break;
	}
	FVector pivot = (pullPoint - actorLocation).ProjectOnToNormal(axisNormal) + actorLocation;

	// Calculate the radius from the attached point to the revolution axis.
	float radius = FVector::Distance(pullPoint, pivot);
	// Crush the problem into two dimensions by solving the right triangle
	// with edge extending along the revolution axis.
	float alongAxisLength = FVector::PointPlaneDist(pullTowards, pullPoint, axisNormal);
	float flatDesiredDistance =
		FMath::Sqrt(desiredDistance * desiredDistance - alongAxisLength * alongAxisLength);
	
	// What is the projected pull vector and it's distance from the pivot?
	FVector flatPullTowards = FVector::PointPlaneProject(pullTowards, pivot, axisNormal);
	float totalDistanceToPivot = FVector::Distance(pivot, flatPullTowards);

	// Now that the problem is projected onto a 2D plane aligned
	// to the revolution axis we use what we know to determine whether
	// there are perfect solutions.
	// Start with the base radians required to get to the shortest
	// snap starting point.
	float deltaRadians = FMath::Acos(
		FVector::DotProduct(
			(pullPoint - pivot).GetSafeNormal(),
			(flatPullTowards - pivot).GetSafeNormal()));

	float beforeRadians = deltaRadians;

	// Do we need to flip the polarity of those radians?
	if (FVector::PointPlaneDist(flatPullTowards, pullPoint,
		FVector::CrossProduct(axisNormal, pullPoint - pivot)) < 0.F)
	{
		deltaRadians *= -1.F;
	}
	if (flatDesiredDistance > totalDistanceToPivot + radius)
	{
		// The distance cannot be this long, spinning 180
		// degrees is as far as we can get.
		deltaRadians += PI;
	}
	else if (flatDesiredDistance < totalDistanceToPivot - radius)
	{
		// The distance cannot get any closer. The closest
		// snap is already optimal so we don't need to do anything.
	}
	else
	{
		// There is a perfect solution and we can use the law of
		// cosines to solve the SSS triangle.
		float addedRadians = FMath::Acos(
			(radius * radius + totalDistanceToPivot * totalDistanceToPivot - flatDesiredDistance * flatDesiredDistance)
			/
			(2.F * radius * totalDistanceToPivot));

		// Should we add or subtract? To minimize effort
		// we should do the operation that undoes the existing delta.
		if (deltaRadians > 0.F)
			deltaRadians -= addedRadians;
		else
			deltaRadians += addedRadians;
	}
	// Apply the reaction.
	AddActorLocalRotation(FQuat(axisNormal, deltaRadians));
}
