#include "WalkState.h"
#include "../Player/PlayerPawn.h"

UWalkState::UWalkState() { }
UWalkState::~UWalkState() { }
UWalkState* UWalkState::instance;
UWalkState* UWalkState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<UWalkState>();
	}
	return instance;
}

#pragma region State Events

void UWalkState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Walking";
	UState::Initialize(pawn);
}

void UWalkState::OnStateEnter() 
{
	player->stateName = this->stateName;
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
		player->SetState(URunState::GetInstance());
	}
}

void UWalkState::CheckIfPlayerIsTryingToCrouch() {
	if (player->tryingToCrouch) 
	{
		player->SetState(UCrouchState::GetInstance());
	}
}
#pragma endregion