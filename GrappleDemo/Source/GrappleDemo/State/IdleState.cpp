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
	player->collider->SetPhysMaterialOverride(player->collider->stopMat);
}

void UIdleState::StateTick(float deltaTime)
{
	CheckForVelocityChange();
	CheckIfPlayerIsTryingToCrouch();
	CheckIfGrounded();
	HandleJump(player->walkJumpForce, true);
	PlayerLook(deltaTime);	
	UMovementState::CheckStateChangeGrapple();
}

void UIdleState::OnStateExit() 
{
	player->collider->SetPhysMaterialOverride(player->collider->moveMat);
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
