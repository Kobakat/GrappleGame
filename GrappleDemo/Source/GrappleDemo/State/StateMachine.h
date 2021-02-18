#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IdleState.h"
#include "WalkState.h"
#include "RunState.h"
#include "CrouchState.h"
#include "RunSlideState.h"
#include "SlideState.h"
#include "GrappleAirborneState.h"
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
	
	UPROPERTY()
		UState* state;
	UPROPERTY()
		UIdleState* idleState;
	UPROPERTY()
		UWalkState* walkState;
	UPROPERTY()
		URunState* runState;
	UPROPERTY()
		UCrouchState* crouchState;
	UPROPERTY()
		URunSlideState* runSlideState;
	UPROPERTY()
		USlideState* slideState;
	UPROPERTY()
		UGrappleAirborneState* grappleAirborneState;

private:
	APlayerPawn* player;
};

