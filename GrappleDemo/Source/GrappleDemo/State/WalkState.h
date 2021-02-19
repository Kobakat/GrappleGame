// Fill out your copyright notice in the Description page of Project Settings.

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

	virtual void OnStateEnter() override;
	virtual void StateTick(float DeltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	virtual void PlayerMove(float accel, float airControlFactor) override;
	virtual void PlayerLook() override;
	virtual void CheckIfGrounded() override;
	virtual void ClampPlayerVelocity(float max) override;
	virtual void HandleJump(float jumpForce) override;
	virtual FVector ConvertPlayerInputRelativeToCamera() override;

	virtual void CheckIfPlayerIsTryingToCrouch();
private:
	void CheckIfPlayerIsTryingToRun();
};
