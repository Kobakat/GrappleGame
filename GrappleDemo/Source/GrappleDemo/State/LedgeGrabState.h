#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "Engine/EngineTypes.h"
#include "LedgeGrabState.generated.h"

UENUM()
enum ELedgePushType
{
	LPT_Lazy, //The player provides no input, gently push them forward
	LPT_Slow, //The player is walking forward, give them a burst of walk speed,
	LPT_Fast //The player is trying to run forward, give them a burst of running speed
};


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

	void SetLedge(FHitResult newLedge, FHitResult newLedgeTop);

private:
	static ULedgeGrabState* instance;
	FHitResult ledge;
	FHitResult ledgeTop;

	void InitializeLedgeValues();
	void LiftPlayerUp(float deltaTime);
	void PushPlayerForward(float deltaTime);
	
	void DeterminePlayerAction(float deltaTime);

	//Exit positioning
	TEnumAsByte<ELedgePushType> pushType;
	FVector pushDir;
	FVector startLoc;

	FVector topPoint;
	bool bNeedsToPush;
	float pushTimer;
	float liftHeight;
};
