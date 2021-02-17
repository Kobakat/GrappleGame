// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "State.h"
#include "MovementState.generated.h"

/**
 * 
 */
UCLASS()
class GRAPPLEDEMO_API UMovementState : public UState
{
	GENERATED_BODY()
	
public:
	~UMovementState();

	virtual void OnStateEnter() override;
	virtual void StateTick() override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

protected:
	virtual void PlayerMove(float accel, float airControlFactor);
	virtual void PlayerLook();
	virtual void CheckIfGrounded();
	virtual void HandleJump(float jumpForce);
	virtual void ClampPlayerVelocity(float max);
	virtual FVector ConvertPlayerInputRelativeToCamera();

};
