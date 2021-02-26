#include "StateMachine.h"
#include "../Player/PlayerPawn.h"
UStateMachine::UStateMachine() { }
UStateMachine::~UStateMachine() { }


void UStateMachine::Initialize(APlayerPawn* pawn)
{
	this->player = pawn;

	UGrappleAirborneState::GetInstance()->Initialize(pawn);
	UGrappleInstantReelState::GetInstance()->Initialize(pawn);;
	UCrouchState::GetInstance()->Initialize(pawn);;
	UIdleState::GetInstance()->Initialize(pawn);;
	URunSlideState::GetInstance()->Initialize(pawn);;
	URunState::GetInstance()->Initialize(pawn);;
	USlideState::GetInstance()->Initialize(pawn);;
	UWalkState::GetInstance()->Initialize(pawn);;

	this->state = UIdleState::GetInstance();
}

void UStateMachine::SetState(UState* newState)
{
	if (newState != nullptr && this->state != nullptr)
	{
		this->state->OnStateExit();
		this->state = newState;
		this->player->state = this->state;
		this->state->OnStateEnter();
	}
}

void UStateMachine::Tick(float deltaTime)
{
	if (this->state != nullptr)
	{
		this->state->StateTick(deltaTime);
	}
}