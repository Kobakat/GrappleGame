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
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	void CheckForVelocityChange();
	void CheckIfPlayerIsTryingToCrouch();
	
};
