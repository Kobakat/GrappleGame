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

void ATrackPullableGrappleReactor::ApplyPullForce(const FVector force)
{
	// Calculate the new track distance that the platform
	// should be at
	float newTrackDistance = FVector::PointPlaneDist(
		GetActorLocation() + force,
		TrackStart,
		trackAlongNormal
	);
	newTrackDistance = FMath::Clamp(newTrackDistance, 0.0f, trackLength);
	// Apply the new track distance
	SetActorLocation(TrackStart + trackAlongNormal * newTrackDistance);
}

void ATrackPullableGrappleReactor::SetIsPullable(const bool isPullable)
{
	isCurrentlyPullable = isPullable;
}
