#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "Engine/Enginetypes.h"
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
	virtual void PlayerMove(float accel, float airControlFactor) override;
	virtual void CheckIfGrounded() override;

	void SetSlide(FHitResult newSlide);
private:
	static USlideState* instance;
	static FHitResult slide;
	void HandleCameraTransition(float deltaTime);
	void HandleCrouchDown(float deltaTime);

	float crouchTimer;
	float camTimer;
	bool bIsCrouching;
	bool bIsTransitioning;

	FVector slideNormal;

	
};

