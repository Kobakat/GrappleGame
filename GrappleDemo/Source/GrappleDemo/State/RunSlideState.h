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

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	void CheckIfSlideComplete();
	void AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight);

};
