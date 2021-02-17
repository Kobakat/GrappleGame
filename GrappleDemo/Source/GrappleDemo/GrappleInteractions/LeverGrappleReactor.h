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
	UPROPERTY(EditAnywhere, Category = "Lever Parameters")
	FVector LeverOnDirection;
	UPROPERTY(EditAnywhere, Category = "Lever Parameters")
	float ForceThreshold;

private:
	bool isSwitched;

public:
	bool GetIsSwitched();
	virtual void ApplyPullForce(const FVector force) override;
	// Blueprints can respond to switch being triggered.
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnSwitched"))
	void ReceiveOnSwitched(bool switchState);

};
 
