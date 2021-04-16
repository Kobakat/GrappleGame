// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "../GrappleRendering/PolylineCylinderRenderer.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "GrappleGunComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSoundUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGrappleHitUpdated, FVector, hitLocation);

UCLASS(ClassGroup = "Grapple", meta = (BlueprintSpawnableComponent))
class GRAPPLEDEMO_API UGrappleGunComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrappleGunComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	

	// The hook object that lodges into the hooked location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	USceneComponent* GrappleHookEnd;
	// The current state for whether the gun is rendered
	bool isRendered;

	// The starting point for the grapple raycast
	USceneComponent* CastingFromComponent;
	// The current length of the grapple cable
	float Length;
	// True when the hook is shooting towards the target
	float IsShooting;
	// True when the hook is retracting to the gun
	float IsRetracting;
	// Whether the grapple is currently hovered on a grapple surface
	bool CanAttach;
	// True while the grapple hook is attached
	bool IsAttached;
	// The most recent actor hit by a cast to attach to
	AActor* LastHitActor;
	// The location in 3D space of the last cast hit
	FVector LastHitLocation;
	// The current actor that is hooked onto
	AActor* CurrentHookedActor;
	// The location of the attach point, relative to the attached to actor
	FVector LocalAttachedLocation;
	// Stores the current reactor the cable is hooked to
	AGrappleReactor* CurrentReactor;

	// Tracks the state of reeling for audio purposes
	float LastFrameLength;
	bool isReeling;
	bool isUnreeling;
	// Called once when the grapple hook is shot from the gun
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnGrappleShot;
	// Called once when the grapple hook hits a surface
	UPROPERTY(BlueprintAssignable)
	FGrappleHitUpdated OnGrappleHit;
	// Called every tick while the hook is shooting
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnGrappleStartedTraveling;
	// Called every tick while the hook is returning
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnGrappleStoppedTraveling;
	// Called once when the grapple begins reeling in
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnStartedReelingIn;
	// Called once when the grapple stops reeling in
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnStoppedReelingIn;
	// Called once when the grapple begins reeling out
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnStartedReelingOut;
	// Called once when the grapple stops reeling out
	UPROPERTY(BlueprintAssignable)
	FSoundUpdated OnStoppedReelingOut;

public:

	// Defines where the grapple hook end returns to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	USceneComponent* GunEnd;

	// The maximum fire range of the grapple
	UPROPERTY(EditAnywhere, Category = "Grapple Gun")
	float FireRange;
	// The minimum cable length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	float MinLength;
	// The maximum cable length
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	float MaxLength;
	// Controls how fast the shot animates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	float ShotSpeed;
	// Actors that are ignored in the grapple raycast
	TArray<AActor*> IgnoredActors;
	// The polyline used to render the cable
	APolylineCylinderRenderer* Polyline;

	// Sets the scene component that the cast starts from (should be a camera)
	void SetCastingFromComponent(USceneComponent* CastingFrom);
	// Gets whether the grapple gun can currently attach
	bool GetCanAttach();
	// Gets whether the grapple gun is a transition animation state
	bool GetIsAnimating();
	// Gets the current length of the cable
	UFUNCTION(BlueprintCallable)
	float GetLength();
	// Gets the attached location in world space
	FVector GetAttachedLocation();
	// Gets the end of the gun barrel where the rope starts
	FVector GetGunEnd();
	// Signals the start of the shoot animation to attach to a point
	void Attach();
	// Signals the start of the retract animation to detach from a point
	void Detach();
	// Detaches the grapple, skipping the animation
	UFUNCTION(BlueprintCallable)
	void ResetDetach();
	// Applys a force along the grapple cable
	// Trys to pull the grappled object such that the distance
	// between the specified vectors is the desired distance
	void ApplyForce(FVector pullPoint, FVector pullTowards, float desiredDistance);
	// Reels the grapple cable in or out by scene units
	void Reel(float value);
	// Sets whether the grapple gun is renderer and
	// will perform casts against the environment
	void SetIsRendered(bool isRendered);

	// Called every frame to check grapple opportunities
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
