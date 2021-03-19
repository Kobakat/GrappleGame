// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GrappleReactor.h"
#include "TrackPullableGrappleReactor.generated.h"

UCLASS()
class GRAPPLEDEMO_API ATrackPullableGrappleReactor : public AGrappleReactor
{
	GENERATED_BODY()
	
protected:
	bool isCurrentlyPullable;
	// These are stored to make frequent calculations faster
	FVector trackAlongNormal;
	float trackLength;
	// Inspector fields that control the pullable track
	UPROPERTY(EditAnywhere, Category = "Pullable Parameters")
	FVector TrackStart;
	UPROPERTY(EditAnywhere, Category = "Pullable Parameters")
	FVector TrackEnd;

public:
	// Sets default values for this actor's properties
	ATrackPullableGrappleReactor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance) override;
	// Sets whether the actor is current pullable, or locked
	void SetIsPullable(const bool isPullable);
};
