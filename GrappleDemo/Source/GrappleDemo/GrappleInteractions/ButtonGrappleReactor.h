// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrappleReactor.h"
#include "ButtonGrappleReactor.generated.h"

/**
 * 
 */
UCLASS()
class GRAPPLEDEMO_API AButtonGrappleReactor : public AGrappleReactor
{
	GENERATED_BODY()
	
protected:
	bool isPressed;

	UPROPERTY(EditAnywhere, Category = "Lever Parameters")
	bool IsTimer;
	UPROPERTY(EditAnywhere, Category = "Lever Parameters")
	float TimerDuration;

public:

	bool GetIsPressed();

	virtual void Tick(float DeltaTime) override;

	virtual void ApplyPullForce(const FVector force) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnActivated"))
	void ReceiveOnActivated();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeactivated"))
	void ReceiveOnDeactivated();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTimerTick"))
	void ReceiveOnTimerTick(float timeRemaining01, float timeRemainingSeconds);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OnSwitched"))
	void Reset();

};
