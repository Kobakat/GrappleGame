// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MovementState.h"
#include "../../Player/GrappleGunComponent.h"
#include "../../GrappleRendering/PolylineCylinderRenderer.h"
#include "GrappleState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleState : public UMovementState
{
	GENERATED_BODY()
	
protected:
	// Grapple References.
	UGrappleGunComponent* grappleComponent;
	APolylineCylinderRenderer* grapplePolyline;
	// Common grapple leash behaviour.
	bool SolveRestraint();
	// Solves only the wrap part of the behaviour.
	void SolveWrap();

	TArray<FVector> WrapPivots;
	TArray<AActor*> WrapActors;
	int MaxWrapPivots;
	FVector LastFramePlayerLocation;
	int WrapCheckIterations;

public:
	// UMovementState Implementation.
	virtual void Initialize(APlayerPawn* pawn) override;

	virtual void OnStateEnter() override;
	virtual void OnStateExit() override;
};
