// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleState.h"
#include "../../Player/PlayerPawn.h"

void UGrappleState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	// Retrieve the grapple components.
	grappleComponent = pawn->grappleComponent;
	grappleGunStart = pawn->grappleStart;
	//grappleComponent->SetHiddenInGame(true); // TODO
}

bool UGrappleState::SolveGrappleRestraint()
{
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
	return true;
}
