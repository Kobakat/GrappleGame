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
	player->stateName = this->stateName;
}
void UGrappleInstantReelState::OnStateExit()
{
	UGrappleState::OnStateExit();
}

void UGrappleInstantReelState::StateTick(float deltaTime)
{
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook(deltaTime);
	if (!grappleComponent->GetIsAnimating())
	{
		UpdateGrappleRope(deltaTime);
		bool isSolved = SolveRestraint();
		if (!isSolved)
		{
			player->SetState(UWalkState::GetInstance());
		}
	}
	else
		SolveWrap();

	CheckStateChange();
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
	grappleComponent->Reel(-player->instantGrappleSpeed * 5.F * deltaTime);
	if (grappleComponent->GetLength() < player->reelCompleteDistance)
		player->SetState(UGrappleAirborneState::GetInstance());
}

#pragma endregion