#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "IdleState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UIdleState : public UMovementState
{
	GENERATED_BODY()

public:
	UIdleState();
	~UIdleState();

	virtual void OnStateEnter() override;
	virtual void StateTick() override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	//virtual void PlayerMove(float accel, float airControlFactor);
	virtual void PlayerLook();
	virtual void CheckIfGrounded();
	virtual void HandleJump(float jumpForce);
	//virtual void ClampPlayerVelocity(float max);
	//virtual FVector ConvertPlayerInputRelativeToCamera();
private:
	void CheckForVelocityChange();
	void CheckIfPlayerIsTryingToCrouch();
	
};
