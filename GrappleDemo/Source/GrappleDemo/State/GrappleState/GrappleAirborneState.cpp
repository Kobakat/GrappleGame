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
	UGrappleState::OnStateEnter();
	player->stateName = this->stateName;
	player->tryingToGrapple = false;
}
void UGrappleAirborneState::OnStateExit()
{
	UGrappleState::OnStateExit();
	grappleComponent->Detach();
}

void UGrappleAirborneState::StateTick(float deltaTime)
{
	if (player->bIsGrounded)
		PlayerMove(player->runAcceleration, 100.F);
	else
		PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook(deltaTime);
	HandleJump(player->walkJumpForce);
	HandleGrappleInput();

	CheckIfGrounded(player->groundCheckDistance);
	ClampPlayerVelocity(player->bIsGrounded ? player->walkMaxSpeed : player->airborneMaxSpeed);

	bool isSolved =	SolveRestraint();
	if (!isSolved)
	{
		player->SetState(UWalkState::GetInstance());
	}

	CheckStateChange();
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
	grappleComponent->Reel(5.F * player->reelingAxis);
}

#pragma endregion
