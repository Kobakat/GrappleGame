#include "RunState.h"
#include "../Player/PlayerPawn.h"

URunState::URunState() { }
URunState::~URunState() { }

void URunState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Running";
}

void URunState::OnStateEnter()
{
	player->state = this->stateName;
}

void URunState::StateTick(float deltaTime)
{
	CheckIfGrounded();
	CheckifPlayerWantsToSlide();
	CheckIfPlayerStopsRunning();
	HandleJump(player->runJumpForce);
	PlayerMove(player->runAcceleration, player->runAirControlPercentage);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->runMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void URunState::OnStateExit() { }

#pragma endregion

#pragma region Game Logic

void URunState::CheckIfPlayerStopsRunning()
{
	if (!player->tryingToSprint)
	{
		player->stateMachine->SetState(player->stateMachine->walkState);
	}
}

void URunState::CheckifPlayerWantsToSlide() 
{
	if (player->tryingToCrouch && player->bIsGrounded) 
	{
		player->stateMachine->SetState(player->stateMachine->runSlideState);
	}
}

#pragma endregion
