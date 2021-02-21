#include "State.h"
#include "../Player/PlayerPawn.h"

UState::UState() { }
UState::~UState() { }

void UState::OnStateEnter() { }
void UState::StateTick(float deltaTime)  { }
void UState::OnStateExit() { }
void UState::Initialize(APlayerPawn* pawn) 
{ 
	this->player = pawn;
	this->AddToRoot(); //Prevents the state from getting garbage collected
}
