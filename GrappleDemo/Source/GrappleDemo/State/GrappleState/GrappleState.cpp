// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleState.h"
#include "DrawDebugHelpers.h"
#include "../../Player/PlayerPawn.h"

void UGrappleState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	// Retrieve the grapple components.
	grappleComponent = pawn->grappleComponent;
	grappleGunStart = pawn->grappleStart;
	grapplePolyline = pawn->GrapplePolyline;
	WrapCheckIterations = 10;
}

void UGrappleState::OnStateEnter()
{
	UMovementState::OnStateEnter();
	WrapPivots.Empty();
	grapplePolyline->SetAllPoints(TArray<FVector>());
	grapplePolyline->PushPoint(grappleComponent->GetAttachedLocation());
	grapplePolyline->PushPoint(grappleGunStart->GetComponentLocation());
	LastFramePlayerLocation = player->camera->GetComponentLocation();
}
void UGrappleState::OnStateExit()
{
	UMovementState::OnStateExit();
	grapplePolyline->SetAllPoints(TArray<FVector>());
}

bool UGrappleState::SolveGrappleRestraint()
{
	SolveWrap();
	float lengthLeft = grappleComponent->GetCableLength();
	if (WrapPivots.Num() > 0)
		lengthLeft -= FVector::Distance(grappleComponent->GetAttachedLocation(), WrapPivots[0]);
	for (int i = 1; i < WrapPivots.Num(); i++)
		lengthLeft -= FVector::Distance(WrapPivots[i], WrapPivots[i - 1]);


	// Get a vector pointing from the grapple
	// gun to the grapple hook point.
	FVector ropeVector;
	if (WrapPivots.Num() > 0)
		ropeVector = WrapPivots.Last() - player->camera->GetComponentLocation();
	else
		ropeVector = grappleComponent->GetAttachedLocation() - player->camera->GetComponentLocation();
	// Get the difference in length from the rope
	// radius to the grapple gun.
	float lengthDifference = ropeVector.Size() - lengthLeft;
	// If we are outside the radius:
	if (lengthDifference > 0.F)
	{
		FVector ropeDirection = ropeVector.GetSafeNormal();
		// Get a vector for the tensile offset.
		FVector tensileDelta = ropeDirection * lengthDifference;
		grappleComponent->ApplyForce(-tensileDelta);

		// Pull the player back in, and offset that force
		// by invoking the grapple to apply the opposite
		// force on a GrappleReactor (if available).
		player->AddActorWorldOffset(tensileDelta, true);

		if (WrapPivots.Num() > 0)
			ropeVector = WrapPivots.Last() - player->camera->GetComponentLocation();
		else
			ropeVector = grappleComponent->GetAttachedLocation() - player->camera->GetComponentLocation();

		if (ropeVector.Size() - grappleComponent->GetCableLength() > 1.F)
		{
			return false;
		}
		else
		{
			// Get the current velocity and the point on the sphere surface.
			FVector velocity = player->collider->GetPhysicsLinearVelocity();
			FVector tangentPoint = player->camera->GetComponentLocation();
			// Project the player velocity such that it is tangent to the
			// sphere of the rope radius.
			player->collider->SetPhysicsLinearVelocity(
				FVector::PointPlaneProject(
					tangentPoint + velocity,
					tangentPoint,
					-ropeDirection)
				- tangentPoint);
		}
	}
	return true;
}

void UGrappleState::SolveWrap()
{
	// Grab key elements.
	UWorld* world = grappleComponent->GetWorld();
	FVector hook = grappleComponent->GetAttachedLocation();
	FVector gun = grappleGunStart->GetComponentLocation();
	FVector capsule = player->camera->GetComponentLocation();
	// Create space for raycast elements.
	FHitResult GrappleHitPoint;
	FCollisionQueryParams CollisionParams;
	// Handle any unwrapping.
	// TODO this has a bug where the unwrap process can be shortcircuited.
	// The wrap pivots need to hold normal data to fix this.
	if (WrapPivots.Num() > 0)
	{
		// Step backwards, checking if each pivot can be unwrapped.
		for (int i = WrapPivots.Num() - 1; i >= 0; i--)
		{
			FVector nextPivot = ((i == 0) ? hook : WrapPivots[i - 1]);
			// Is there a clear line around this wrap point?
			// If so unwind the pivot, otherwise stop checking pivots.
			if (!world->LineTraceSingleByChannel(GrappleHitPoint, nextPivot, capsule, ECC_GameTraceChannel4, CollisionParams))
			{
				FVector normalSideCheck = (WrapPivots[i] - capsule).ProjectOnTo(nextPivot - capsule) + capsule;
				if (!world->LineTraceSingleByChannel(GrappleHitPoint, WrapPivots[i], normalSideCheck, ECC_GameTraceChannel4, CollisionParams))
				{
					WrapPivots.Pop();
					grapplePolyline->PopPoint();
				}
			}
			else
				break;
		}
	}
	// Handle any wrapping.
	// Wrapping is always relative to the final wrap pivot.
	FVector Start = capsule;
	FVector End = ((WrapPivots.Num() == 0) ? hook : WrapPivots.Last());
	int cycles = 0;
	// Keep checking the cable line until there is a free path to the grapple end.
	// Wrap as many times as is needed.
	while (world->LineTraceSingleByChannel(GrappleHitPoint, End, Start, ECC_GameTraceChannel4, CollisionParams))
	{
		// Avoid getting trapped in this while loop.
		cycles++;
		if (cycles > 10)
			break;
		// Initialize variables for bisection.
		float currentDistance = 0.5F;
		float cutDistance = 0.5F;
		FHitResult lastHitResult;
		// Run bisection to iteratively approach the actual edge
		// of the geometry to wrap around.
		for (int i = 0; i < WrapCheckIterations; i++)
		{
			// Move towards the last frame's position that was
			// known to be non-obscured.
			cutDistance *= 0.5F;
			Start = FMath::Lerp(capsule, LastFramePlayerLocation, currentDistance);
			// If we are still hitting then continue further towards last frame.
			if (world->LineTraceSingleByChannel(GrappleHitPoint, End, Start, ECC_GameTraceChannel4, CollisionParams))
			{
				currentDistance += cutDistance;
				lastHitResult = GrappleHitPoint;
			}
			else
			{
				// If we cleared then step back.
				currentDistance -= cutDistance;
			}
		}
		// Use the last found hit, should be pretty close to the edge,
		// and add some clearence (otherwise future casts will get trapped in the mesh).
		FVector normal = (LastFramePlayerLocation - capsule).GetSafeNormal();
		FVector wrapPoint = lastHitResult.Location + normal * 5.F;
		WrapPivots.Add(wrapPoint);
		grapplePolyline->SetLastPoint(wrapPoint);
		grapplePolyline->PushPoint(FVector::ZeroVector);
		End = WrapPivots.Last();
	}
	// Update the grapple polyline to reflect hook and player movement.
	grapplePolyline->SetFirstPoint(hook);
	grapplePolyline->SetLastPoint(grappleGunStart->GetComponentLocation());
	// Update last stored location.
	LastFramePlayerLocation = capsule;
}
