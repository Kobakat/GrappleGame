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
	//grappleComponent->SetHiddenInGame(true); // TODO
	WrapCheckIterations = 10;
}

void UGrappleState::OnStateEnter()
{
	WrapPivots.Empty();
	LastFramePlayerLocation = grappleGunStart->GetComponentLocation();
}

bool UGrappleState::SolveGrappleRestraint()
{
	SolveWrap();

	// Get a vector pointing from the grapple
	// gun to the grapple hook point.
	FVector ropeVector =
		grappleComponent->GetAttachedLocation()
		- grappleGunStart->GetComponentLocation();
	// Get the difference in length from the rope
	// radius to the grapple gun.
	float lengthDifference = ropeVector.Size()
		- grappleComponent->GetCableLength();
	// If we are outside the radius:
	if (lengthDifference > 0.0F)
	{
		FVector ropeDirection = ropeVector.GetSafeNormal();
		// Get a vector for the tensile offset.
		FVector tensileDelta = ropeDirection * lengthDifference;
		grappleComponent->ApplyForce(-tensileDelta);

		// Pull the player back in, and offset that force
		// by invoking the grapple to apply the opposite
		// force on a GrappleReactor (if available).
		player->AddActorWorldOffset(tensileDelta, true);

		ropeVector =
			grappleComponent->GetAttachedLocation()
			- grappleGunStart->GetComponentLocation();

		if (ropeVector.Size() - grappleComponent->GetCableLength() > 1.F)
		{
			LastFramePlayerLocation = grappleGunStart->GetComponentLocation();
			return false;
		}
		else
		{
			// Get the current velocity and the point on the sphere surface.
			FVector velocity = player->playerCollider->GetPhysicsLinearVelocity();
			FVector tangentPoint = player->GetActorLocation();
			// Project the player velocity such that it is tangent to the
			// sphere of the rope radius.
			player->playerCollider->SetPhysicsLinearVelocity(
				FVector::PointPlaneProject(
					tangentPoint + velocity,
					tangentPoint,
					-ropeDirection)
				- tangentPoint);
		}
	}
	LastFramePlayerLocation = grappleGunStart->GetComponentLocation();
	return true;
}

void UGrappleState::SolveWrap()
{
	// TODO partial implementation is left here in case we
	// end up using it.

	// Grab key elements.
	UWorld* world = grappleComponent->GetWorld();
	FVector hook = grappleComponent->GetAttachedLocation();
	FVector gun = grappleGunStart->GetComponentLocation();
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
			if (!world->LineTraceSingleByChannel(GrappleHitPoint, nextPivot, gun, ECC_GameTraceChannel4, CollisionParams))
				WrapPivots.Pop();
			else
				break;
		}
	}

	// Handle any wrapping.
	// Wrapping is always relative to the final wrap pivot.
	FVector Start = gun;
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
			Start = FMath::Lerp(gun, LastFramePlayerLocation, currentDistance);
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
		FVector wrapPoint = lastHitResult.Location + (LastFramePlayerLocation - gun).GetSafeNormal() * 10.F;
		WrapPivots.Add(wrapPoint);
		End = WrapPivots.Last();
	}

	// Draw debug lines for the wrapped cable.
	if (WrapPivots.Num() == 0)
	{
		DrawDebugLine(world,
			hook, gun,
			FColor(255, 0, 0),
			false, 0.F, 0.F, 2.F
		);
	}
	else
	{
		DrawDebugLine(world,
			hook, WrapPivots[0],
			FColor(255, 0, 0),
			false, 0.F, 0.F, 2.F
		);
		for (int i = 0; i < WrapPivots.Num(); i++)
		{
			if (i != WrapPivots.Num() - 1)
			{
				DrawDebugLine(world,
					WrapPivots[i], WrapPivots[i + 1],
					FColor(255, 0, 0),
					false, 0.F, 0.F, 2.F
				);
			}
			else
			{
				DrawDebugLine(world,
					WrapPivots[i], gun,
					FColor(255, 0, 0),
					false, 0.F, 0.F, 2.F
				);
			}
		}
	}
}
