#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "cringerTest.generated.h"

class APlayerPawn;

UCLASS()
class GRAPPLEDEMO_API UcringerTest : public UStaticMeshComponent
{
	GENERATED_BODY()


public:
	UcringerTest();

	UPROPERTY(EditAnywhere, Category = "Materials")
		UPhysicalMaterial* moveMat;
	UPROPERTY(EditAnywhere, Category = "Materials")
		UPhysicalMaterial* stopMat;
	UPROPERTY(EditAnywhere, Category = "Materials")
		UPhysicalMaterial* runSlideMat;
	UPROPERTY(EditAnywhere, Category = "Materials")
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

