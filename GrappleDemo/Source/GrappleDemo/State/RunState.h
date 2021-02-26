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
	static URunState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	static URunState* instance;
	void CheckIfPlayerStopsRunning();
	void CheckifPlayerWantsToSlide();
};

