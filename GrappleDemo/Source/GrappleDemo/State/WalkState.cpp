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

void UWalkState::StateTick(float DeltaTime)
{
	CheckIfGrounded();
	CheckIfPlayerIsTryingToRun();
	CheckIfPlayerIsTryingToCrouch();
	HandleJump(player->walkJumpForce);
	PlayerMove(player->walkAcceleration, player->walkAirControlPercentage);
	PlayerLook();
	ClampPlayerVelocity(player->walkMaxSpeed);
	UMovementState::CheckStateChangeGrapple();
}

void UWalkState::OnStateExit() { }

#pragma endregion

#pragma region Game Logic
void UWalkState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void UWalkState::PlayerLook() { UMovementState::PlayerLook(); }
void UWalkState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void UWalkState::ClampPlayerVelocity(float accel) { UMovementState::ClampPlayerVelocity(accel); }
void UWalkState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }
FVector UWalkState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }

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