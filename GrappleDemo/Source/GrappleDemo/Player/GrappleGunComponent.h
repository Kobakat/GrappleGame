// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "GrappleGunComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GRAPPLEDEMO_API UGrappleGunComponent : public UActorComponent
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
	// Defines where the grapple hook end returns to
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grapple Gun")
	USceneComponent* GunEnd;

	// The starting point for the grapple raycast
	USceneComponent* CastingFromComponent;
	// The current length of the grapple cable
	float Length;
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

public:	
	// The maximum fire range of the grapple
	UPROPERTY(EditAnywhere, Category = "Grapple Gun")
	float FireRange;
	// The minimum cable length
	UPROPERTY(EditAnywhere, Category = "Grapple Gun")
	float MinLength;
	// The maximum cable length
	UPROPERTY(EditAnywhere, Category = "Grapple Gun")
	float MaxLength;
	// Actors that are ignored in the grapple raycast
	TArray<AActor*> IgnoredActors;

	// Sets the scene component that the cast starts from (should be a camera)
	void SetCastingFromComponent(USceneComponent* CastingFrom);
	// Gets whether the grapple gun can currently attach
	bool GetCanAttach();
	// Gets the current length of the cable
	float GetLength();
	// Gets the attached location in world space
	FVector GetAttachedLocation();
	// Gets the end of the gun barrel where the rope starts
	FVector GetGunEnd();
	// Attaches the grapple to the most recent grapple point
	void Attach();
	// Detaches from the current grapple point
	void Detach();
	// Applys a force along the grapple cable
	// Trys to pull the grappled object such that the distance
	// between the specified vectors is the desired distance
	void ApplyForce(FVector pullPoint, FVector pullTowards, float desiredDistance);
	// Reels the grapple cable in or out by scene units
	void Reel(float value);

	// Called every frame to check grapple opportunities
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
