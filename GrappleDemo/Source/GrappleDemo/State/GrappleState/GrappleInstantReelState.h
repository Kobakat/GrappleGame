#pragma once

#include "CoreMinimal.h"
#include "GrappleState.h"
#include "GrappleInstantReelState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleInstantReelState : public UGrappleState
{
	GENERATED_BODY()
	
private:
	static UGrappleInstantReelState* instance;
	void CheckStateChange();
	void UpdateGrappleRope(float deltaTime);

public:
	static UGrappleInstantReelState* GetInstance();
	virtual void Initialize(APlayerPawn* pawn) override;
	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;
};
