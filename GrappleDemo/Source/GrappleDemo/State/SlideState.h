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
	static USlideState* GetInstance();

	virtual void OnStateEnter() override;
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	static USlideState* instance;
	void CheckIfStillOnSlide();
	void AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight);
};

