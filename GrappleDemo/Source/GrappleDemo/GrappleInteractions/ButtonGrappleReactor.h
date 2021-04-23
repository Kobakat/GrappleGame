// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GrappleReactor.h"
#include "ButtonGrappleReactor.generated.h"

UCLASS()
class GRAPPLEDEMO_API AButtonGrappleReactor : public AGrappleReactor
{
	GENERATED_BODY()
	
protected:
	bool isPressed;
	float startTime;

	UPROPERTY(EditAnywhere, Category = "Button Parameters")
	bool AutoDetachGrapple;
	UPROPERTY(EditAnywhere, Category = "Button Parameters")
	bool IsTimer;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Button Parameters")
	float TimerDuration;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnActivated"))
	void ReceiveOnActivated();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeactivated"))
	void ReceiveOnDeactivated();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnTimerTick"))
	void ReceiveOnTimerTick(float timeRemaining01, float timeRemainingSeconds);

public:
	AButtonGrappleReactor();

	bool GetIsPressed();

	virtual void Hook(FVector location, APlayerPawn* player, UGrappleGunComponent* grappleGun);
	virtual void Tick(float DeltaTime) override;
	virtual void ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance) override;
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "OnSwitched"))
	void Reset();

};
