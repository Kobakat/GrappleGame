// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GrappleReactor.h"
#include "LeverGrappleReactor.generated.h"

UCLASS()
class GRAPPLEDEMO_API ALeverGrappleReactor : public AGrappleReactor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, Category = "Lever Parameters", meta=(
		ClampMin = "1.0", ClampMax = "90.0",
		UIMin = "1.0", UIMax = "90.0"))
	float AngleTolerance = 30.F;

private:
	bool isSwitched;
	float dotThreshold;

public:
	bool GetIsSwitched();
	virtual void ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance) override;
	// Blueprints can respond to switch being triggered.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSwitched"))
	void ReceiveOnSwitched(bool switchState);

};
 
