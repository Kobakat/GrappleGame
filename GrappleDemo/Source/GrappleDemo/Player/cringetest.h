#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "cringetest.generated.h"

class APlayerPawn;

UENUM()
enum ECameraFOVState
{
	Passive,
	Active
};

UENUM()
enum ECameraShakeState
{
	Stopped,
	Shaking
};

UCLASS()
class GRAPPLEDEMO_API Ucringetest : public UCameraComponent
{
	GENERATED_BODY()

public:
	Ucringetest();
	void TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, Category = "Camera Stats | General")
		FVector2D viewLookBounds;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | General")
		float lookSpeed;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | General")
		float slideTransitionTime;

protected:
	void BeginPlay() override;
private:
	APlayerPawn* player;

	void UpdateFOVState();
	void UpdateFOV(const float deltaTime);
	void UpdateShakeState();
	void UpdateShake(const float deltaTime, const float amplitude, const float freq);

#pragma region Designer Props
	//This FOV is used while walking, crouching, and idling
	UPROPERTY(EditAnywhere, Category = "Camera Stats | FOV")
		float FOVPassive;
	//This FOV is used while running, sliding, and swinging
	UPROPERTY(EditAnywhere, Category = "Camera Stats | FOV")
		float FOVActive;
	//How many seconds does it take to adjust the camera FOV when switching to/from active or passive states
	UPROPERTY(EditAnywhere, Category = "Camera Stats | FOV")
		float FOVTransitionTime;
	//Duration of the blend-in, where the oscillation scales from 0 to 1.
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float shakeBlendInTime;
	//Duration of the blend-out, where the oscillation scales from 1 to 0.
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float shakeBlendOutTime;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float passiveAmplitude;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float passiveFrequency;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float activeAmplitude;
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float activeFrequency;

#pragma endregion

	//Shake
	UPROPERTY(VisibleAnywhere, Category = "Camera Stats | State")
		TEnumAsByte<ECameraShakeState> shakeState;

	float initialHeight;
	float shakeInTimer;
	float shakeOutTimer;
	float shakeOffset;
	float shakeAmp;
	float shakeFreq;
	float shakeStartOffset;
	float shakeHeight;
	bool blendingIn;
	bool blendingOut;

	//FOV
	UPROPERTY(VisibleAnywhere, Category = "Camera Stats | State")
		TEnumAsByte<ECameraFOVState> fovState;
	TEnumAsByte<ECameraFOVState> prevFOVState;
	float fovTimer;
	bool fovTransition;
};
