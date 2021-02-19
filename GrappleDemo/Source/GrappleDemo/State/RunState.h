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
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	void CheckIfPlayerStopsRunning();
	void CheckifPlayerWantsToSlide();
};
