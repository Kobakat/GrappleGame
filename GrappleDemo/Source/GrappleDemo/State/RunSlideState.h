#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "RunSlideState.generated.h"

UCLASS()
class GRAPPLEDEMO_API URunSlideState : public UMovementState
{
	GENERATED_BODY()
	
public:
	URunSlideState();
	~URunSlideState();

	static URunSlideState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	static URunSlideState* instance;
	float crouchTimer;
	bool bIsCrouching;
	void CheckIfSlideComplete();
	void HandleCrouchDown(float deltaTime);

};

