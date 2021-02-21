#pragma once

#include "CoreMinimal.h"
#include "CableComponent.h"
#include "GrappleInteractions/GrappleReactor.h"
#include "GrappleComponent.generated.h"

UCLASS()
class GRAPPLEDEMO_API UGrappleComponent : public UCableComponent
{
	GENERATED_BODY()

private:
	float currentCableLength;
	FVector attachedLocation;
	AActor* attachedActor;
	
public:
	UGrappleComponent();
	~UGrappleComponent();

	UPROPERTY(EditAnywhere, Category = "Grapple Options")
		float grappleFireRange;
	UPROPERTY(EditAnywhere, Category = "Grapple Options")
		float grappleMaxDistance;
	UPROPERTY(EditAnywhere, Category = "Grapple Options")
		float grappleReelSpeed;
	//Use this to make the rope seem more tight than it actually is (0.7 - 0.9 are ideal)
	UPROPERTY(EditAnywhere, Category = "Grapple Options")
		float grappleVisualMultiplier;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Accessors.
	FVector GetAttachedLocation();
	float GetCableLength();

	void ApplyForce(FVector force);
	void Reel(float value);
	// function that takes in vector and attaches endpoint of grapple to that
	void Attach(FVector vector, AActor* actor);
	void Detach();

	AGrappleReactor* grappleReactor;
};
