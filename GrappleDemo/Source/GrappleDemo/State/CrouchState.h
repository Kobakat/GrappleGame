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
	static UCrouchState* GetInstance();
	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	virtual void HandleJump(float jumpForce) override;

private:
	static UCrouchState* instance;
	void CheckIfPlayerIsTryingToStand();
	void HandleCrouchDown(float deltaTime);
	float crouchTimer;
	bool bIsCrouching;
};

