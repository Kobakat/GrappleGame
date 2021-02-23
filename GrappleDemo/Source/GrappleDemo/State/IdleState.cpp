#include "IdleState.h"
#include "../Player/PlayerPawn.h"

UIdleState::UIdleState() { }
UIdleState::~UIdleState() { }
UIdleState* UIdleState::instance;
UIdleState* UIdleState::GetInstance() 
{
	if (instance == nullptr)
	{
		instance = NewObject<UIdleState>();
	}
	return instance;
}

#pragma region State Events

void UIdleState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Idle";
	UState::Initialize(pawn);
}

void UIdleState::OnStateEnter()
{
	player->stateName = this->stateName;
}

void UIdleState::StateTick(float deltaTime)
{
	CheckForVelocityChange();
	CheckIfPlayerIsTryingToCrouch();
	CheckIfGrounded();
	HandleJump(player->walkJumpForce);
	PlayerLook(deltaTime);

	player->playerCollider->SetPhysMaterialOverride(player->stopMat);
	UMovementState::CheckStateChangeGrapple();
}

void UIdleState::OnStateExit() 
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
}

#pragma endregion

#pragma region Game Logic

void UIdleState::CheckForVelocityChange() 
{
	if (player->moveVector.SizeSquared() > 0.01) 
	{
		player->SetState(UWalkState::GetInstance());
	}
}

void UIdleState::CheckIfPlayerIsTryingToCrouch() 
{
	if (player->tryingToCrouch)
	{
		player->SetState(UCrouchState::GetInstance());
	}
}

#pragma endregion
