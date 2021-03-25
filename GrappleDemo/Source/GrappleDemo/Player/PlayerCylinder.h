#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCylinder.generated.h"

class APlayerPawn;

UCLASS()
class GRAPPLEDEMO_API UPlayerCylinder : public UStaticMeshComponent
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* moveMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* stopMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* runSlideMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* noFricMat;

	FVector previousVelocity;

	FVector bounds;
	FHitResult CrouchHitPoint;
	TArray<FHitResult> GroundHits;
	TArray<FHitResult> LedgeHits;

	bool bOnSlide;

	bool bNeedsToStand;
	float standUpTimer;

	void TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool CheckIfGrounded();
	bool CheckIfTryingToStand();
	bool CheckIfLedgeGrabEligible();
	bool CheckIfLedgeGrabEligible(FVector playerMoveVector);

protected:
	void BeginPlay() override;
private:
	APlayerPawn* player;
	void HandleStandUp(float deltaTime);
	FVector CalculateBounds();


};
