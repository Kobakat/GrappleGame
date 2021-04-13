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

UENUM()
enum ELedgeClimbType
{
	LCT_UnderWaist,
	LCT_AboveWaist,		
	LCT_AboveHead,
	LCT_Airborne,
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

	void SetLedge(FHitResult newLedge);

private:
	static ULedgeGrabState* instance;
	static FHitResult ledge;

	void InitializeLedgeValues();
	void LiftPlayerUp(float deltaTime);
	void PushPlayerForward(float deltaTime);
	
	void DeterminePlayerAction(float deltaTime);
	void UnderWaistAction(float deltaTime);
	void AboveWaistAction(float deltaTime);
	void AboveHeadAction(float deltaTime);
	void AirborneAction(float deltaTime);

	void BobCamera(float deltaTime);

	//Tick Position
	TEnumAsByte<ELedgeClimbType> climbType;

	//Exit positioning
	TEnumAsByte<ELedgePushType> pushType;
	FVector pushDir;
	FVector startLoc;

	//Camera Rotation
	FVector lookPoint;
	FVector goalRot;
	bool bNeedsToTurn;
	bool bNeedsToBob;
	bool bNeedsToPush;
	float bobTimer;
	float pushTimer;
	//Common
	float liftHeight;
	float climbDistance;
};
