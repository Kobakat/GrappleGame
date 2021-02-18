// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MovementState.h"
#include "../GrappleComponent.h"
#include "GrappleAirborneState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleAirborneState : public UMovementState
{
	GENERATED_BODY()
	
private:
	// Utility Functions.
	void CheckStateChange();
	void HandleGrappleInput();
	void UpdateGrappleRope();
	// Grapple References.
	UGrappleComponent* grappleComponent;
	USceneComponent* grappleGunStart;

public:
	// UMovementState Implementation.
	virtual void Initialize(APlayerPawn* pawn) override;
	virtual void OnStateEnter() override;
	virtual void StateTick() override;
	virtual void OnStateExit() override;
};
