// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "GrappleInteractions/GrappleReactor.h"
#include "GrappleComponent.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleComponent : public UCableComponent
{
	GENERATED_BODY()

private:
	float currentCableLength;
	FVector attachedLocation;
	AActor* attachedActor;
	
public:
	UGrappleComponent();
	// TODO since this component must be added at runtime
	// these values need to be passed in from player pawn inspector.
	UPROPERTY(EditAnywhere)
		float reelMultiplier;
	UPROPERTY(EditAnywhere)
		float maxGrappleLength;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Accessors.
	FVector GetAttachedLocation();
	float GetCableLength();

	void ApplyForce(FVector force);
	void Reel(float value);
	// function that takes in vector and attaches endpoint of grapple to that
	void Attach(FVector vector, AActor* actor);
	void Detach();

	AGrappleReactor* grappleReactor;
};
