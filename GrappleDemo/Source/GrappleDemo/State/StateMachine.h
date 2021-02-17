#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IdleState.h"
#include "WalkState.h"
#include "RunState.h"
#include "CrouchState.h"
#include "RunSlideState.h"
#include "SlideState.h"
#include "StateMachine.generated.h"

UCLASS()
class GRAPPLEDEMO_API UStateMachine : public UObject
{
	GENERATED_BODY()

public:

	UStateMachine();
	~UStateMachine();

	void SetState(UState* newState);
	void Tick();
	void Initialize(APlayerPawn* pawn);
	
	UState* state;
	UIdleState* idleState;
	UWalkState* walkState;
	URunState* runState;
	UCrouchState* crouchState;
	URunSlideState* runSlideState;
	USlideState* slideState;

private:
	APlayerPawn* player;
};

