#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PlayerCapsule.generated.h"

class APlayerPawn;

UCLASS()
class GRAPPLEDEMO_API UPlayerCapsule : public UCapsuleComponent
{
	GENERATED_BODY()


public:
	UPlayerCapsule();

	UPROPERTY(EditAnywhere, Category = "Physic Materials")
		UPhysicalMaterial* moveMat;
	UPROPERTY(EditAnywhere, Category = "Physic Materials")
		UPhysicalMaterial* stopMat;
	UPROPERTY(EditAnywhere, Category = "Physic Materials")
		UPhysicalMaterial* runSlideMat;
	UPROPERTY(EditAnywhere, Category = "Physic Materials")
		UPhysicalMaterial* noFricMat;

	float halfHeight;
	FVector previousVelocity;
	FVector bounds;

	TArray<FHitResult> CrouchHits;
	TArray<FHitResult> GroundHits;
	TArray<FHitResult> LedgeHits;
	FHitResult LedgeTop;

	bool bOnSlide;

	bool bNeedsToStand;
	float standUpTimer;

	void HandleStandUp(float deltaTime);
	void TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;	
	bool CheckIfGrounded();
	bool CheckIfTryingToStand();
	bool CheckIfLedgeGrabEligible();
	bool CheckIfLedgeGrabEligible(FVector playerMoveVector);
	bool CheckIfStepUp(float& outHeight);
	float GetCrouchTime(float targetScale);

protected:
	void BeginPlay() override;
private:
	APlayerPawn* player;
	FVector CalculateBounds();
	
};
