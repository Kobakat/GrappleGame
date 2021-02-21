#include "RunState.h"
#include "../Player/PlayerPawn.h"

URunState::URunState() { }
URunState::~URunState() { }
URunState* URunState::instance;
URunState* URunState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<URunState>();
	}
	return instance;
}

#pragma region State Events

void URunState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Running";
	UState::Initialize(pawn);
}

void URunState::OnStateEnter()
{
	player->stateName = this->stateName;
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
		player->SetState(UWalkState::GetInstance());
	}
}

void URunState::CheckifPlayerWantsToSlide() 
{
	if (player->tryingToCrouch && player->bIsGrounded) 
	{
		player->SetState(URunSlideState::GetInstance());
	}
}

#pragma endregion
