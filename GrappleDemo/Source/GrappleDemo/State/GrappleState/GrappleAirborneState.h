#pragma once

#include "CoreMinimal.h"
#include "GrappleState.h"
#include "../../GrappleComponent.h"
#include "GrappleAirborneState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleAirborneState : public UGrappleState
{
	GENERATED_BODY()
	
private:
	static UGrappleAirborneState* instance;
	void CheckStateChange();
	void HandleGrappleInput();

public:
	static UGrappleAirborneState* GetInstance();
	virtual void Initialize(APlayerPawn* pawn) override;
	virtual void OnStateEnter() override;
	virtual void StateTick(float DeltaTime) override;
	virtual void OnStateExit() override;
};
