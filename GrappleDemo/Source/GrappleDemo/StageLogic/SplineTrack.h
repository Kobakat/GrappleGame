// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "TrackEndBehaviour.h"
#include "SplineTrack.generated.h"

// TODO make base class for both the tracks
UCLASS()
class GRAPPLEDEMO_API ASplineTrack : public AActor
{
	GENERATED_BODY()
	
private:
	bool isSafe;

public:	
	// Sets default values for this actor's properties
	ASplineTrack();

	// The current speed of the track
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Parameters")
	float CurrentSpeed;
	// Whether to orient objects to their direction on the track
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Parameters")
	bool OrientObjectsToTrack;
	// Whether objects should flip around when traveling backwards on the track
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Parameters")
	bool FlipOrientation;

protected:
	// Used to access the driving spline
	UPROPERTY(EditAnywhere, Category = "Track Parameters")
	AActor* ActorWithSpline;
	// The end behaviour of objects moving along the spline
	UPROPERTY(EditAnywhere, Category = "Track Parameters")
	TEnumAsByte<ETrackEndBehaviour> EndBehaviour;
	// The objects to attach to the spline
	UPROPERTY(EditAnywhere, Category = "Track Parameters")
	TArray<AActor*> AttachedObjects;

	USplineComponent* Spline;
	TArray<float> ObjectLocations;
	TArray<bool> ObjectDirections;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to initialize the track behaviour
	void Initialize();
};
