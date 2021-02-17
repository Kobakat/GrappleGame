// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleReactor.generated.h"

UCLASS()
class GRAPPLEDEMO_API AGrappleReactor : public AActor
{
	GENERATED_BODY()
	
protected:
	// Store the state for current hook
	bool isHookedOnto;
	// TODO this should be a reference to the grapplegun class
	FTransform* hookedTransform;

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
	void ReceiveOnForceApplied(FVector force);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Call these methods to control the hooked state
	virtual void Hook();
	virtual void Unhook();
	// Applys a directional force to this grapple reactor
	// Sub classes, blueprints will process this
	virtual void ApplyPullForce(const FVector force);

};
