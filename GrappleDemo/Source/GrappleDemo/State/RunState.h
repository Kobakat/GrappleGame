#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "RunState.generated.h"


UCLASS()
class GRAPPLEDEMO_API URunState : public UMovementState
{
	GENERATED_BODY()

public:
	URunState();
	~URunState();

	virtual void OnStateEnter() override;
	virtual void StateTick() override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	virtual void PlayerMove(float accel, float airControlFactor) override;
	virtual void PlayerLook() override;
	virtual void CheckIfGrounded() override;
	virtual void ClampPlayerVelocity(float max) override;
	virtual void HandleJump(float jumpForce) override;
	virtual FVector ConvertPlayerInputRelativeToCamera() override;
private:
	void CheckIfPlayerStopsRunning();
	void CheckifPlayerWantsToSlide();
};
