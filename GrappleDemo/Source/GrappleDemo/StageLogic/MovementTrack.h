// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrackEndBehaviour.h"
#include "MovementTrack.generated.h"

UCLASS()
class GRAPPLEDEMO_API AMovementTrack : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovementTrack();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Track Parameters")
	float currentSpeed;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Track Parameters")
	TArray<AActor*> AttachedObjects;
	UPROPERTY(EditAnywhere, Category = "Track Parameters")
	TEnumAsByte<ETrackEndBehaviour> EndBehaviour;

	// Stores the calculated segments based on track nodes.
	TArray<float> segmentLengths;
	TArray<FVector> segmentNormals;
	TArray<FVector> segmentStarts;
	// Stores the current distance along segments for actors.
	TArray<int> attachedIndices;
	TArray<float> currentDistance;

public:	
	virtual void Tick(float DeltaTime) override;

};
