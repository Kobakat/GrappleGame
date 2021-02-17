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

void URunState::StateTick()
{
	CheckIfGrounded();
	CheckifPlayerWantsToSlide();
	CheckIfPlayerStopsRunning();
	HandleJump(player->runJumpForce);
	PlayerMove(player->runAcceleration, player->runAirControlPercentage);
	PlayerLook();
	ClampPlayerVelocity(player->runMaxSpeed);
}

void URunState::OnStateExit() { }

#pragma endregion

#pragma region Game Logic
void URunState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void URunState::PlayerLook() { UMovementState::PlayerLook(); }
void URunState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void URunState::ClampPlayerVelocity(float max) { UMovementState::ClampPlayerVelocity(max); }
void URunState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }
FVector URunState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }

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
