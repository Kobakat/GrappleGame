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
enum ELedgeCameraState
{
	LCS_Turning, //The player camera is turning to look at the ledge corner
	LCS_Tilting, //The camera has crest the corner and is now tilting to be parallel to the ground
	LCS_Done //The camera won't do anything
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

	void InitializePositionValues();
	void InitializeCameraValues();

	void DeterminePlayerAction(float deltaTime);
	void DetermineCameraAction(float deltaTime);

	void LiftPlayerUp(float deltaTime);
	void PushPlayerForward();

	void TurnCamera(float deltaTime);
	void TiltCamera(float deltaTime);

	//Player positioning
	TEnumAsByte<ELedgePushType> pushType;
	FVector pushDir;
	FVector startLoc;

	//Camera Animation
	TEnumAsByte<ELedgeCameraState> cameraState;
	FVector lookDirStart;
	FVector lookDirFinal;
	FVector lookCorner;

	//Common
	float liftHeight;
	bool bClimbComplete;
};
