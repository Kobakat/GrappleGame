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

void UIdleState::StateTick()
{
	CheckForVelocityChange();
	CheckIfGrounded();
	HandleJump(player->walkJumpForce);
	PlayerLook();
}

void UIdleState::OnStateExit() 
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
}

#pragma endregion

#pragma region Game Logic
//void UIdleState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void UIdleState::PlayerLook() { UMovementState::PlayerLook(); }
void UIdleState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
//void UIdleState::ClampPlayerVelocity(float max) { UMovementState::ClampPlayerVelocity(max); }
void UIdleState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }
//FVector UIdleState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }

void UIdleState::CheckForVelocityChange() 
{
	if (!player->playerCollider->GetPhysicsLinearVelocity().IsZero() || player->moveVector.SizeSquared() > 0) 
	{
		player->stateMachine->SetState(player->stateMachine->walkState);
	}
}

#pragma endregion
