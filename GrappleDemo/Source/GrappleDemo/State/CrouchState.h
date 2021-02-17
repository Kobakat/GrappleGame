#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "CrouchState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UCrouchState : public UMovementState
{
	GENERATED_BODY()
	
public:
	UCrouchState();
	~UCrouchState();

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
	void CheckIfPlayerIsTryingToStand();
	void AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight);
};
