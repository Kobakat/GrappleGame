// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleReactor.generated.h"

class APlayerPawn;
class UGrappleGunComponent;

UCLASS()
class GRAPPLEDEMO_API AGrappleReactor : public AActor
{
	GENERATED_BODY()
	
protected:
	// Store the state for current hook
	bool isHookedOnto;
	FVector localHookPosition;
	// TODO this should be a reference to the grapplegun class
	FTransform* hookedTransform;
	APlayerPawn* hookedPlayer;
	UGrappleGunComponent* hookedGun;

public:	
	// Sets default values for this actor's properties
	AGrappleReactor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// This function forces the grapple hook to detach
	UFUNCTION(BlueprintCallable, Category = "Grapple Functions")
	void ForceDetach();
	// Allow blueprints to directly respond to these basic functions
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnHooked"))
	void ReceiveOnHooked();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnUnhooked"))
	void ReceiveOnUnhooked();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnForceApplied"))
	void ReceiveOnForceApplied();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Call these methods to control the hooked state
	virtual void Hook(FVector location, APlayerPawn* player, UGrappleGunComponent* grappleGun);
	virtual void Unhook();
	// Attempts to solve a pulling force from a global pull point
	// towards a target pull point. The solver will attempt to
	// move the reactor such that the local pull point is the
	// desired distance away from the towards location.
	virtual void ApplyPullForce(const FVector pullPoint, const FVector pullTowards, const float desiredDistance);
};
