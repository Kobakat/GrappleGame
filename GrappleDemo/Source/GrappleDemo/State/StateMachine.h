#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IdleState.h"
#include "WalkState.h"
#include "RunState.h"
#include "CrouchState.h"
#include "RunSlideState.h"
#include "SlideState.h"
#include "./GrappleState/GrappleAirborneState.h"
#include "GrappleState/GrappleInstantReelState.h"
#include "StateMachine.generated.h"

UCLASS()
class GRAPPLEDEMO_API UStateMachine : public UObject
{
	GENERATED_BODY()

public:

	UStateMachine();
	~UStateMachine();

	void SetState(UState* newState);
	void Tick(float deltaTime);
	void Initialize(APlayerPawn* pawn);
	
	UPROPERTY()
		UState* state;

private:
	APlayerPawn* player;
};

