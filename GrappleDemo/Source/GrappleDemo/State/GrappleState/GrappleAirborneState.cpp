#include "GrappleAirborneState.h"
#include "../../Player/PlayerPawn.h"

UGrappleAirborneState* UGrappleAirborneState::instance;

UGrappleAirborneState* UGrappleAirborneState::GetInstance()
{
	if (instance == nullptr) 
	{
		instance = NewObject<UGrappleAirborneState>();
	}

	return instance;
}

#pragma region State Events

void UGrappleAirborneState::Initialize(APlayerPawn* pawn)
{
	instance->stateName = "Swinging";
	UGrappleState::Initialize(pawn);
}

void UGrappleAirborneState::OnStateEnter()
{
	player->stateName = this->stateName;
	grappleComponent->SetHiddenInGame(false);
	player->tryingToGrapple = false;
}
void UGrappleAirborneState::OnStateExit()
{
	grappleComponent->Detach();
	grappleComponent->SetHiddenInGame(true);
}

void UGrappleAirborneState::StateTick(float deltaTime)
{
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook(deltaTime);
	CheckStateChange();
	HandleJump(player->walkJumpForce);
	HandleGrappleInput();

	bool isSolved =	SolveGrappleRestraint();
	if (!isSolved)
	{
		player->SetState(UWalkState::GetInstance());
	}
	CheckIfGrounded();
	ClampPlayerVelocity(player->airborneMaxSpeed);
}

#pragma endregion

#pragma region Game Logic

void UGrappleAirborneState::CheckStateChange()
{
	// If the grapple or jump button is pressed then release
	// the grapple and return to walk state.
	if (player->tryingToGrapple ||
		(player->tryingToJump && !player->bIsGrounded))
		player->SetState(UWalkState::GetInstance());
}
void UGrappleAirborneState::HandleGrappleInput()
{
	// Apply the reeling input to the grapple rope.
	grappleComponent->Reel(player->reelingAxis);
}

#pragma endregion
