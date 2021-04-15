// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineTrack.h"
#include "Logging/MessageLog.h" 

// Sets default values
ASplineTrack::ASplineTrack()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASplineTrack::BeginPlay()
{
	Super::BeginPlay();
	// Has a spline been specified?
	if (ActorWithSpline != nullptr)
	{
		// If so attempt to get the spline component.
		UActorComponent* component =
			ActorWithSpline->FindComponentByClass<USplineComponent>();
		if (component != nullptr)
			Spline = Cast<USplineComponent>(component);
	}
#if WITH_EDITOR
	// Check editor fields to avoid
	// null reference crashes
	isSafe = true;
	FMessageLog PIELogger = FMessageLog(FName("PIE"));
	if (Spline == nullptr)
	{
		isSafe = false;
		PIELogger.Error(FText::FromString(
			"A SplineTrack is missing an actor reference with a spline component. Did not run."));
	}
	if (isSafe)
	{
		for (AActor* actor : AttachedObjects)
		{
			if (actor == nullptr)
			{
				isSafe = false;
				PIELogger.Error(FText::FromString(
					"A spline track has null references in Attached Objects. Did not run."));
				break;
			}
		}
	}
	if (isSafe)
	{
#endif
		// Find the nearest snap on the line for each actor
		for (AActor* actor : AttachedObjects)
		{
			// Find the key for the location on the track
			float key = Spline->FindInputKeyClosestToWorldLocation(
				actor->GetActorLocation());
			// Get the surrounding spline node indices
			int priorKeyIndex = key;
			int followingKeyIndex = key + 1;
			// Calculate the distance using interpolation
			float baseDistance =
				Spline->GetDistanceAlongSplineAtSplinePoint(priorKeyIndex);
			float segmentLength =
				Spline->GetDistanceAlongSplineAtSplinePoint(followingKeyIndex) - baseDistance;
			float distance = baseDistance + segmentLength * (key - priorKeyIndex);
			// Calculate the angle against the current spline direction
			// this will set the initial direction of the object
			float angle = FMath::Acos(
				FVector::DotProduct(
					actor->GetActorForwardVector(),
					Spline->GetWorldDirectionAtDistanceAlongSpline(distance)));
			// Initialize object state
			ObjectLocations.Add(distance);
			// Flip the direction if the object is
			// oriented against the spline
			ObjectDirections.Add(angle < PI * 0.5F);
		}
#if WITH_EDITOR
	}
#endif
}

// Called every frame
void ASplineTrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Calculate the spline length
	// TODO this might not have to be done per
	// frame maybe add a static spline bool

#if WITH_EDITOR
	if (isSafe)
	{
#endif
		float splineEndLength = Spline->GetSplineLength();
		float distanceDelta = CurrentSpeed * DeltaTime;
		for (int i = 0; i < AttachedObjects.Num(); i++)
		{
			AActor* actor = AttachedObjects[i];
			if (ObjectDirections[i])
			{
				ObjectLocations[i] += distanceDelta;
				if (ObjectLocations[i] > splineEndLength)
				{
					switch (EndBehaviour)
					{
					case ETrackEndBehaviour::LoopAround:
						ObjectLocations[i] -= splineEndLength;
						break;
					case ETrackEndBehaviour::PingPong:
						ObjectDirections[i] = false;
						ObjectLocations[i] -= ObjectLocations[i] - splineEndLength;
						break;
					case ETrackEndBehaviour::StopAtEnds:
						ObjectLocations[i] = splineEndLength;
						break;
					}
				}
			}
			else
			{
				ObjectLocations[i] -= distanceDelta;
				if (ObjectLocations[i] < 0.F)
				{
					switch (EndBehaviour)
					{
					case ETrackEndBehaviour::LoopAround:
						ObjectLocations[i] += splineEndLength;
						break;
					case ETrackEndBehaviour::PingPong:
						ObjectDirections[i] = true;
						ObjectLocations[i] *= -1.F;
						break;
					case ETrackEndBehaviour::StopAtEnds:
						ObjectLocations[i] = 0.F;
						break;
					}
				}
			}
			// Orient the transform.
			actor->SetActorLocation(Spline->GetWorldLocationAtDistanceAlongSpline(ObjectLocations[i]));
			if (OrientObjectsToTrack)
			{
				FVector direction = Spline->GetWorldDirectionAtDistanceAlongSpline(ObjectLocations[i]);
				if (FlipOrientation && !ObjectDirections[i])
					direction *= -1.F;
				actor->SetActorRotation(direction.Rotation().Quaternion());
			}

		}
#if WITH_EDITOR
	}
#endif
}
