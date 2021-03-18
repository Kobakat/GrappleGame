// Fill out your copyright notice in the Description page of Project Settings.


#include "TrackPullableGrappleReactor.h"

ATrackPullableGrappleReactor::ATrackPullableGrappleReactor()
{
	isCurrentlyPullable = true;
}

void ATrackPullableGrappleReactor::BeginPlay()
{
	trackAlongNormal = (TrackEnd - TrackStart).GetSafeNormal();
	trackLength = (TrackEnd - TrackStart).Size();

	// TODO the following code should be abstracted into
	// a helper method

	// Set the initial position of the reactor to snap to the track
	SetActorLocation(
		(GetActorLocation() - TrackStart).ProjectOnTo(trackAlongNormal)
		+ TrackStart
	);
	// Check to see if the snap extrapolates the track limits
	float distanceSquared = FVector::DistSquared(TrackStart, TrackEnd);
	float startDistanceSquared = FVector::DistSquared(GetActorLocation(), TrackStart);
	float endDistanceSquared = FVector::DistSquared(GetActorLocation(), TrackEnd);
	// A tolerance is used here since an interpolant snap will always yield a really close comparison
	if (startDistanceSquared + endDistanceSquared > distanceSquared - 0.005f)
	{
		// Snap to the nearer end point
		if (startDistanceSquared < endDistanceSquared)
			SetActorLocation(TrackStart);
		else
			SetActorLocation(TrackEnd);
	}
}

void ATrackPullableGrappleReactor::ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance)
{
	// Get the projected snap of the pull path.
	FVector perpendicularSnap =
		(pullTowards - pullPoint).ProjectOnToNormal(trackAlongNormal) + pullPoint;
	float snapDistance = FVector::Distance(perpendicularSnap, pullTowards);

	// Is it impossible to meet the distance?
	float distanceDelta;
	if (snapDistance >= desiredDistance)
	{
		// If so then the desired snap location is just the snap point.
		distanceDelta = FVector::PointPlaneDist(perpendicularSnap, pullPoint, trackAlongNormal);
	}
	else
	{
		// Otherwise we solve the right triangle
		// to get the distance in either direction.
		float distanceFromSnap =
			FMath::Sqrt(desiredDistance * desiredDistance - snapDistance * snapDistance);
		// Take whichever distance along is less effort.
		float distanceLeft = FVector::PointPlaneDist(
			perpendicularSnap + trackAlongNormal * distanceFromSnap,
			pullPoint, trackAlongNormal);
		float distanceRight = FVector::PointPlaneDist(
			perpendicularSnap - trackAlongNormal * distanceFromSnap,
			pullPoint, trackAlongNormal);
		if (FMath::Abs(distanceLeft) < FMath::Abs(distanceRight))
			distanceDelta = distanceLeft;
		else
			distanceDelta = distanceRight;
	}
	
	float priorDistanceAlong = FVector::PointPlaneDist(GetActorLocation(), TrackStart, trackAlongNormal);
	// Lock the distance in the track range.
	float distanceAlong = FMath::Clamp(priorDistanceAlong + distanceDelta, 0.F, trackLength);
	// Apply the new track distance.
	SetActorLocation(TrackStart + trackAlongNormal * distanceAlong);
}

void ATrackPullableGrappleReactor::SetIsPullable(const bool isPullable)
{
	isCurrentlyPullable = isPullable;
}
