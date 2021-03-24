// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../MovementState.h"
#include "../../GrappleComponent.h"
#include "../../GrappleRendering/PolylineCylinderRenderer.h"
#include "GrappleState.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleState : public UMovementState
{
	GENERATED_BODY()
	
private:
	void SolveWrap();

protected:
	// Grapple References.
	UGrappleComponent* grappleComponent;
	USceneComponent* grappleGunStart;
	USceneComponent* grappleClaw;
	APolylineCylinderRenderer* grapplePolyline;
	// Common grapple leash behaviour.
	bool SolveRestraint();

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
