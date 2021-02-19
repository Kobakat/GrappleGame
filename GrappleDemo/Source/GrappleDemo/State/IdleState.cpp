#include "IdleState.h"
#include "../Player/PlayerPawn.h"

UIdleState::UIdleState() { }
UIdleState::~UIdleState() { }

void UIdleState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Idle";
}

void UIdleState::OnStateEnter()
{
	player->state = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->stopMat);
}

void UIdleState::StateTick(float DeltaTime)
{
	CheckForVelocityChange();
	CheckIfPlayerIsTryingToCrouch();
	CheckIfGrounded();
	HandleJump(player->walkJumpForce);
	PlayerLook();

	UMovementState::CheckStateChangeGrapple();
}

void UIdleState::OnStateExit() 
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
}

#pragma endregion

#pragma region Game Logic

void UIdleState::PlayerLook() { UMovementState::PlayerLook(); }
void UIdleState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void UIdleState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }


void UIdleState::CheckForVelocityChange() 
{
	if (player->moveVector.SizeSquared() > 0.01) 
	{
		player->stateMachine->SetState(player->stateMachine->walkState);
	}
}

void UIdleState::CheckIfPlayerIsTryingToCrouch() 
{
	if (player->tryingToCrouch)
	{
		player->stateMachine->SetState(player->stateMachine->crouchState);
	}
}

#pragma endregion
