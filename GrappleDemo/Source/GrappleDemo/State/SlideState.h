#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "SlideState.generated.h"


UCLASS()
class GRAPPLEDEMO_API USlideState : public UMovementState
{
	GENERATED_BODY()

public:
	USlideState();
	~USlideState();

	virtual void OnStateEnter() override;
	virtual void StateTick(float DeltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	//virtual void PlayerMove(float accel, float airControlFactor) override;
	virtual void PlayerLook() override;
	virtual void CheckIfGrounded() override;
	virtual void ClampPlayerVelocity(float max) override;
	virtual void HandleJump(float jumpForce) override;
	virtual FVector ConvertPlayerInputRelativeToCamera() override;

private:
	void CheckIfStillOnSlide();
	void AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight);
};
