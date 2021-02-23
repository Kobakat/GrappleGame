#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MovementState.h"
#include "WalkState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UWalkState : public UMovementState
{
	GENERATED_BODY()
	
public:
	UWalkState();
	~UWalkState();
	static UWalkState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	static UWalkState* instance;
	void CheckIfPlayerIsTryingToRun();
	void CheckIfPlayerIsTryingToCrouch();
};

