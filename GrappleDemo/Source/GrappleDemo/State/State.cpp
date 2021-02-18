#include "State.h"
#include "../Player/PlayerPawn.h"

UState::UState() { }
UState::~UState() { }

void UState::OnStateEnter() { }
void UState::StateTick() { }
void UState::OnStateExit() { }
void UState::Initialize(APlayerPawn* pawn) { this->player = pawn; }
