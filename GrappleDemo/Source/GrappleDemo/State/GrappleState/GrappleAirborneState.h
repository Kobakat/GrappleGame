// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrappleState.h"
#include "../../GrappleComponent.h"
#include "GrappleAirborneState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleAirborneState : public UGrappleState
{
	GENERATED_BODY()
	
private:
	// Utility Functions.
	void CheckStateChange();
	void HandleGrappleInput();

public:
	// UMovementState Implementation.
	virtual void Initialize(APlayerPawn* pawn) override;
	virtual void OnStateEnter() override;
	virtual void StateTick(float DeltaTime) override;
	virtual void OnStateExit() override;
};
