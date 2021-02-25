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
	player->stateName = this->stateName;
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

#pragma endregion

#pragma region Game Logic

void UGrappleInstantReelState::CheckStateChange()
{
	// If the grapple or jump button is pressed then release
	// the grapple and return to walk state.
	if (player->tryingToInstantReel || player->tryingToJump)
		player->SetState(UWalkState::GetInstance());
}
void UGrappleInstantReelState::UpdateGrappleRope(float deltaTime)
{
	grappleComponent->Reel(-player->instantGrappleSpeed * deltaTime);
	if (grappleComponent->GetCableLength() < player->reelCompleteDistance)
		player->SetState(UGrappleAirborneState::GetInstance());
}

#pragma endregion