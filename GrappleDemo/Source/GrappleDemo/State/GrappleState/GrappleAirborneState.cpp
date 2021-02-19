// Fill out your copyright notice in the Description page of Project Settings.

#include "GrappleAirborneState.h"
#include "../../Player/PlayerPawn.h"

void UGrappleAirborneState::Initialize(APlayerPawn* pawn)
{
	UGrappleState::Initialize(pawn);
	this->stateName = "Grapple Airborne";
}

void UGrappleAirborneState::OnStateEnter()
{
	player->state = this->stateName;
	grappleComponent->SetHiddenInGame(false);
	player->tryingToGrapple = false;
}
void UGrappleAirborneState::OnStateExit()
{
	grappleComponent->Detach();
	grappleComponent->SetHiddenInGame(true);
}

void UGrappleAirborneState::StateTick(float DeltaTime)
{
	// TODO: should these have different behaviours
	// while grapple airborne?
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook();
	// Locally defined behaviour:
	CheckStateChange();
	UMovementState::HandleJump(player->walkJumpForce); //TODO
	HandleGrappleInput();
	UpdateGrappleRestraint();
	UMovementState::CheckIfGrounded();
}
void UGrappleAirborneState::CheckStateChange()
{
	// If the grapple or jump button is pressed then release
	// the grapple and return to walk state.
	if (player->tryingToGrapple || 
		(player->tryingToJump && !player->bIsGrounded))
		player->stateMachine->SetState(player->stateMachine->walkState);
}
void UGrappleAirborneState::HandleGrappleInput()
{
	// Apply the reeling input to the grapple rope.
	grappleComponent->Reel(player->reelingAxis);
}
