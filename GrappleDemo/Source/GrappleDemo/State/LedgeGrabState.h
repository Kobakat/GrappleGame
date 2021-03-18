#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "LedgeGrabState.generated.h"

UCLASS()
class GRAPPLEDEMO_API ULedgeGrabState : public UMovementState
{
	GENERATED_BODY()

public:
	ULedgeGrabState();
	~ULedgeGrabState();
	static ULedgeGrabState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:

	void DeterminePlayerAction(float deltaTime);
	void LiftPlayerUp(float deltaTime);
	void PushPlayerForward(float deltaTime);

	float climbTimer;
	float pushTimer;
	float liftHeight;

	FVector pushDir;
	FVector startLoc;
	bool bClimbComplete;
	bool bPushComplete;
	static ULedgeGrabState* instance;
};
