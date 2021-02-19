// Fill out your copyright notice in the Description page of Project Settings.

#include "GrappleInstantReelState.h"
#include "../../Player/PlayerPawn.h"

void UGrappleInstantReelState::Initialize(APlayerPawn* pawn)
{
	UGrappleState::Initialize(pawn);
	this->stateName = "Grapple Instant Reel";
}

void UGrappleInstantReelState::OnStateEnter()
{
	player->state = this->stateName;
	grappleComponent->SetHiddenInGame(false);
}
void UGrappleInstantReelState::OnStateExit()
{
	grappleComponent->SetHiddenInGame(true);
}

void UGrappleInstantReelState::StateTick(float deltaTime)
{
	CheckStateChange();
	PlayerLook(deltaTime);
	UpdateGrappleRope(deltaTime);
	UpdateGrappleRestraint();

	UMovementState::CheckStateChangeGrapple();
}
void UGrappleInstantReelState::CheckStateChange()
{
	// If the grapple or jump button is pressed then release
	// the grapple and return to walk state.
	if (player->tryingToInstantReel || player->tryingToJump)
		player->stateMachine->SetState(player->stateMachine->walkState);
}
void UGrappleInstantReelState::UpdateGrappleRope(float deltaTime)
{
	grappleComponent->Reel(-player->instantGrappleSpeed * deltaTime);
	if (grappleComponent->GetCableLength() < player->reelCompleteDistance)
		player->stateMachine->SetState(player->stateMachine->grappleAirborneState);
}
