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
	static UIdleState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	static UIdleState* instance;
	void CheckForVelocityChange();
	void CheckIfPlayerIsTryingToCrouch();
	
};
