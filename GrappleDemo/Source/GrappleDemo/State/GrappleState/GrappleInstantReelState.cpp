#include "GrappleInstantReelState.h"
#include "../../Player/PlayerPawn.h"
UGrappleInstantReelState* UGrappleInstantReelState::instance;
UGrappleInstantReelState* UGrappleInstantReelState::GetInstance() 
{
	if (instance == nullptr)
	{
		instance = NewObject<UGrappleInstantReelState>();
	}
	return instance;
}

#pragma region State Events

void UGrappleInstantReelState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Instant Reel";
	UGrappleState::Initialize(pawn);
}

void UGrappleInstantReelState::OnStateEnter()
{
	UGrappleState::OnStateEnter();
	if (player->isHookshotStyle)
		player->collider->SetEnableGravity(false);
	player->stateName = this->stateName;
}
void UGrappleInstantReelState::OnStateExit()
{
	UGrappleState::OnStateExit();
	if (player->isHookshotStyle)
		player->collider->SetEnableGravity(true);
}

void UGrappleInstantReelState::StateTick(float deltaTime)
{
	PlayerLook(deltaTime);
	if (!grappleComponent->GetIsAnimating())
	{
		if (player->isHookshotStyle)
			player->collider->SetPhysicsLinearVelocity(FVector::ZeroVector);
		else
		{
			if (player->bGrounded)
				PlayerMove(player->runAcceleration, 100.F);
			else
				PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
		}
		SolveRestraint();
		UpdateGrappleRope(deltaTime);
	}

	CheckStateChange();
	UMovementState::CheckStateChangeGrapple();
}

#pragma endregion

#pragma region Game Logic

void UGrappleInstantReelState::CheckStateChange()
{
	// Release the grapple if the player jumps
	if (player->tryingToJump)
		player->SetState(UWalkState::GetInstance());
	if (!grappleComponent->GetIsAnimating())
		if (grappleComponent->GetLength() <= grappleComponent->MinLength)
			player->SetState(UGrappleAirborneState::GetInstance());
	if (player->collider->CheckIfLedgeGrabEligible())
		player->SetState(ULedgeGrabState::GetInstance());
}
void UGrappleInstantReelState::UpdateGrappleRope(float deltaTime)
{
	grappleComponent->Reel(-player->instantGrappleSpeed * 5.F * deltaTime);
	if (grappleComponent->GetLength() < player->reelCompleteDistance)
		player->SetState(UGrappleAirborneState::GetInstance());
}

#pragma endregion