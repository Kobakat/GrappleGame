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
	UPROPERTY()
		UGrappleAirborneState* test1;
	UPROPERTY()
		UGrappleInstantReelState* test2;
	UPROPERTY()
		UCrouchState* test3;
	UPROPERTY()
		UIdleState* test4;
	UPROPERTY()
		URunSlideState* test5;
	UPROPERTY()
		URunState* test6;
	UPROPERTY()
		USlideState* test7;
	UPROPERTY()
		UWalkState* test8;

private:
	APlayerPawn* player;
};

