// Fill out your copyright notice in the Description page of Project Settings.

#include "MovementTrack.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "../Utilities/SingleLinkedComponent.h"

// Sets default values.
AMovementTrack::AMovementTrack()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void AMovementTrack::BeginPlay()
{
	Super::BeginPlay();
	// Retrieve the path nodes from the scene component
	// children on this actor.
	TArray<USceneComponent*> SceneComponents;
	TArray<USceneComponent*> TempComponents;
	USceneComponent* current = RootComponent;
	bool isAChildComponent = true;
	while (isAChildComponent)
	{
		current->GetChildrenComponents(false, TempComponents);
		if (TempComponents.Num() > 0)
		{
			current = TempComponents[0];
			SceneComponents.Add(current);
		}
		else
			isAChildComponent = false;
	}

	int nodeCount = SceneComponents.Num();
	// How many segments do we need? If this platform route
	// ping pongs we will double the segments to store the
	// backwards path data (instead of having more state per object).
	int totalSegments = nodeCount - 1;
	if (EndBehaviour == ETrackEndBehaviour::PingPong)
		totalSegments *= 2;

	// Initialize state collections for the static path data.
	segmentLengths.Init(0.F, totalSegments);
	segmentNormals.Init(FVector::ZeroVector, totalSegments);
	segmentStarts.Init(FVector::ZeroVector, totalSegments);
	// Initialize state collections each object attached to the path.
	attachedIndices.Init(0, AttachedObjects.Num());
	currentDistance.Init(0.F, AttachedObjects.Num());

	// Precalculate the important path data along each node.
	for (int i = 0; i < nodeCount - 1; i++)
	{
		// Get the start of the segment from the node position.
		segmentStarts[i] = SceneComponents[i]->GetComponentLocation();
		// Get the distance between this node and the next node.
		segmentLengths[i] = FVector::Distance(
			SceneComponents[i + 1]->GetComponentLocation(),
			SceneComponents[i]->GetComponentLocation());
		// Get the normalalized direction vector for this segment.
		segmentNormals[i] =
			(SceneComponents[i + 1]->GetComponentLocation() -
				SceneComponents[i]->GetComponentLocation()).GetSafeNormal();
	}
	if (EndBehaviour == ETrackEndBehaviour::PingPong)
	{
		// Add the backwards path components
		// if ping pong movement is requested.
		for (int i = 0; i < nodeCount - 1; i++)
		{
			segmentStarts[nodeCount + i - 1] =
				SceneComponents[nodeCount - i - 1]->GetComponentLocation();
			// These values are copied from what was previously
			// calculated. The directions are flipped.
			segmentLengths[nodeCount + i - 1] =
				segmentLengths[nodeCount - i - 2];
			segmentNormals[nodeCount + i - 1] =
				-segmentNormals[nodeCount - i - 2];
		}
	}

	// Snap each object to the nearest location on the track.
	for (int i = 0; i < AttachedObjects.Num(); i++)
	{
		FVector location = AttachedObjects[i]->GetActorLocation();
		// Set up comparison state.
		int closestSnapIndex;
		FVector closestSnapLocation;
		float closestSnap = MAX_flt;
		// Look through each of the segments for
		// the closest snap.
		for (int j = 0; j < nodeCount - 1; j++)
		{
			// Use vector projection to find the snap.
			FVector snap =
				(location - segmentStarts[j]).ProjectOnToNormal(segmentNormals[j])
				+ segmentStarts[j];
			// If this snap is better than save it.
			float snapDistance = FVector::Distance(snap, location);
			if (snapDistance < closestSnap)
			{
				closestSnap = snapDistance;
				closestSnapLocation = snap;
				closestSnapIndex = j;
			}
		}
		// Apply the best snap state to this object.
		attachedIndices[i] = closestSnapIndex;
		currentDistance[i] = FVector::Distance(closestSnapLocation, segmentStarts[closestSnapIndex]);
	}
}

// Called every frame.
void AMovementTrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Get the travel of the actors during this tick.
	float delta = GetWorld()->GetDeltaSeconds() * currentSpeed;
	// Effect the movement for each attached actor.
	for (int i = 0; i < AttachedObjects.Num(); i++)
	{
		// Translate the actor position relative to its segment.
		currentDistance[i] += delta;
		// Check if the segment number has gone down.
		while (currentDistance[i] < 0.F)
		{
			attachedIndices[i]--;
			// Is this the end of the loop?
			if (attachedIndices[i] < 0)
			{
				// If the end behaviour is to stop, then lock
				// this actor to the start.
				if (EndBehaviour == ETrackEndBehaviour::StopAtEnds)
				{
					attachedIndices[i] = 0;
					currentDistance[i] = 0.F;
				}
				// Otherwise wrap them around to the other and
				// of the segments.
				else
				{
					attachedIndices[i] = segmentStarts.Num() - 1;
					currentDistance[i] += segmentLengths[attachedIndices[i]];
				}
			}
			// Adjust the current distance when stepped over.
			else
				currentDistance[i] += segmentLengths[attachedIndices[i]];
		}
		// Check if the segment has advanced.
		while (currentDistance[i] > segmentLengths[attachedIndices[i]])
		{
			currentDistance[i] -= segmentLengths[attachedIndices[i]];
			attachedIndices[i]++;
			// Is this the end of the loop?
			if (attachedIndices[i] > segmentStarts.Num() - 1)
			{
				if (EndBehaviour == ETrackEndBehaviour::StopAtEnds)
				{
					attachedIndices[i] = segmentStarts.Num() - 1;
					currentDistance[i] = segmentLengths[attachedIndices[i]];
				}
				else
					attachedIndices[i] = 0;
			}
		}
		// Apply the new location for this actor along the path.
		AttachedObjects[i]->SetActorLocation(
			segmentStarts[attachedIndices[i]] +
			segmentNormals[attachedIndices[i]] * currentDistance[i]);
	}
}
