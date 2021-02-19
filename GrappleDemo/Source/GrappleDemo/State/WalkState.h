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
	virtual void StateTick(float deltaTime) override;
	virtual void OnStateExit() override;

	virtual void Initialize(APlayerPawn* pawn) override;

private:
	void CheckIfPlayerIsTryingToRun();
	void CheckIfPlayerIsTryingToCrouch();
};
