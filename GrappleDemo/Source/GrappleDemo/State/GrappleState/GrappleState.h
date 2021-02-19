// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MovementState.h"
#include "../../GrappleComponent.h"
#include "GrappleState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleState : public UMovementState
{
	GENERATED_BODY()
	
protected:
	// Grapple References.
	UGrappleComponent* grappleComponent;
	USceneComponent* grappleGunStart;
	// Common grapple leash behaviour.
	void UpdateGrappleRestraint();

public:
	// UMovementState Implementation.
	virtual void Initialize(APlayerPawn* pawn) override;
};
