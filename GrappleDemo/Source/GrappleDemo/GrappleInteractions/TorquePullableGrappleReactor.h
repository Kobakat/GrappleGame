// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Math/Axis.h"
#include "GrappleReactor.h"
#include "TorquePullableGrappleReactor.generated.h"

/**
 * 
 */
UCLASS()
class GRAPPLEDEMO_API ATorquePullableGrappleReactor : public AGrappleReactor
{
	GENERATED_BODY()
	
protected:
	// The minimum rotation degrees for this reactor.
	UPROPERTY(EditAnywhere, Category = "Torque Reactor")
	float DegreesLowerLimit;
	// The maximum rotation degrees for this reactor.
	UPROPERTY(EditAnywhere, Category = "Torque Reactor")
	float DegreesUpperLimit;
	// The local axis to revolve around.
	UPROPERTY(EditAnywhere, Category = "Torque Reactor")
	TEnumAsByte<EAxis::Type> Axis;

public:
	// Sets default values for this actor's properties
	ATorquePullableGrappleReactor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void ApplyPullForce(const FVector force) override;
};
