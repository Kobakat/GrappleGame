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
	player->collider->SetEnableGravity(false);
	player->stateName = this->stateName;
}
void UGrappleInstantReelState::OnStateExit()
{
	UGrappleState::OnStateExit();
	player->collider->SetEnableGravity(true);
}

void UGrappleInstantReelState::StateTick(float deltaTime)
{
	PlayerLook(deltaTime);
	if (!grappleComponent->GetIsAnimating())
	{
		player->collider->SetPhysicsLinearVelocity(FVector::ZeroVector);
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
}
void UGrappleInstantReelState::UpdateGrappleRope(float deltaTime)
{
	grappleComponent->Reel(-player->instantGrappleSpeed * 5.F * deltaTime);
	if (grappleComponent->GetLength() < player->reelCompleteDistance)
		player->SetState(UGrappleAirborneState::GetInstance());
}

#pragma endregion