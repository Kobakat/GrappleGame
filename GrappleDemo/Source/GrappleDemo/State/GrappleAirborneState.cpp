// Fill out your copyright notice in the Description page of Project Settings.

#include "GrappleAirborneState.h"
#include "../Player/PlayerPawn.h"

void UGrappleAirborneState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Grapple Airborne";
	// Retrieve the grapple components.
	grappleComponent = pawn->grappleComponent;
	grappleGunStart = pawn->grappleStart;
	grappleComponent->SetHiddenInGame(true);
}

void UGrappleAirborneState::OnStateEnter()
{
	player->state = this->stateName;
	grappleComponent->SetHiddenInGame(false);
}
void UGrappleAirborneState::OnStateExit()
{
	grappleComponent->Detach();
	grappleComponent->SetHiddenInGame(true);
}

void UGrappleAirborneState::StateTick()
{
	// TODO: should these have different behaviours
	// while grapple airborne?
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook();
	// Locally defined behaviour:
	CheckStateChange();
	HandleGrappleInput();
	UpdateGrappleRope();
}

void UGrappleAirborneState::CheckStateChange()
{
	// If the grapple or jump button is pressed then release
	// the grapple and return to walk state.
	if (/*player->IsTryingToGrapple() ||*/ player->tryingToJump)
		player->stateMachine->SetState(player->stateMachine->walkState);
}
void UGrappleAirborneState::HandleGrappleInput()
{
	// Apply the reeling input to the grapple rope.
	grappleComponent->Reel(player->reelingAxis);
}
void UGrappleAirborneState::UpdateGrappleRope()
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
		// Pull the player back in, and offset that force
		// by invoking the grapple to apply the opposite
		// force on a GrappleReactor (if available).
		player->AddActorWorldOffset(tensileDelta);
		grappleComponent->ApplyForce(-tensileDelta);

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
