#include "StateMachine.h"
UStateMachine::UStateMachine()
{
}

UStateMachine::~UStateMachine()
{
}

void UStateMachine::Initialize(APlayerPawn* pawn)
{
	this->player = pawn;

	//Create all the states
	idleState = NewObject<UIdleState>();
	walkState = NewObject<UWalkState>();
	runState = NewObject<URunState>();
	crouchState = NewObject<UCrouchState>();
	slideState = NewObject<USlideState>();
	runSlideState = NewObject <URunSlideState>();

	//Initialize any new states here
	idleState->Initialize(this->player);
	walkState->Initialize(this->player);
	runState->Initialize(this->player);
	crouchState->Initialize(this->player);
	slideState->Initialize(this->player);
	runSlideState->Initialize(this->player);
	
	//Set the default starting state here
	this->state = idleState;
}

void UStateMachine::SetState(UState* newState)
{
	if (newState != nullptr && state != nullptr)
	{
		this->state->OnStateExit();
		this->state = newState;
		this->state->OnStateEnter();
	}
}

void UStateMachine::Tick()
{
	if (state != nullptr)
	{
		this->state->StateTick();
	}
}