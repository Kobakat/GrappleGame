#include "WalkState.h"
#include "../Player/PlayerPawn.h"

#pragma region State Events
UWalkState::UWalkState() { }
UWalkState::~UWalkState() { }

void UWalkState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Walking";
}

void UWalkState::OnStateEnter() 
{
	player->state = this->stateName;
}

void UWalkState::StateTick(float deltaTime)
{
	CheckIfGrounded();
	CheckIfPlayerIsTryingToRun();
	CheckIfPlayerIsTryingToCrouch();
	HandleJump(player->walkJumpForce);
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->walkMaxSpeed);
	UMovementState::CheckStateChangeGrapple();
}

void UWalkState::OnStateExit() { }

#pragma endregion

#pragma region Game Logic

void UWalkState::CheckIfPlayerIsTryingToRun()
{
	if (player->tryingToSprint)
	{
		player->stateMachine->SetState(player->stateMachine->runState);
	}
}

void UWalkState::CheckIfPlayerIsTryingToCrouch() {
	if (player->tryingToCrouch) 
	{
		player->stateMachine->SetState(player->stateMachine->crouchState);
	}
}
#pragma endregion