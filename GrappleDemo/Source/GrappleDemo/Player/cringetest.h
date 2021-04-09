#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "cringetest.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStepAudio);

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

	//How quickly the camera turns during the ledge grab state
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Ledge Grab")
		float ledgeTurnSpeed;
	//How quickly the camera dips when climbing a ledge above the player's head
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Ledge Grab")
		float ledgeDipSpeed;
	//How far the camera dips when climbing a ledge above the player's head
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Ledge Grab")
		float ledgeDipAmplitude;

	float baseHeight;

protected:
	void BeginPlay() override;
private:
	APlayerPawn* player;

	void UpdateFOVState();
	void UpdateFOV(const float deltaTime);
	void UpdateShakeState();
	void UpdateShake(const float deltaTime, const float amplitude, const float freq, const float sideAmp);

	float GetFOVChangeTime(float targetFOV);

	UPROPERTY(BlueprintAssignable)
		FStepAudio OnWalkStep;
	UPROPERTY(BlueprintAssignable)
		FStepAudio OnRunStep;

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
	//Max distance from the orgin the camera moves when the player walks
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float passiveAmplitude;
	//Max rotation from the orgin the camera tilts when the player walks
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float passiveRockAmplitude;
	//How quickly the camera moves when the player walks
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float passiveFrequency;
	//Max distance from the orgin the camera moves when the player runs
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float activeAmplitude;
	//Max rotation from the orgin the camera tilts when the player runs
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float activeRockAmplitude;
	//How quickly the camera moves when the player runs
	UPROPERTY(EditAnywhere, Category = "Camera Stats | Shake", meta = (ClampMin = "0.0"))
		float activeFrequency;
#pragma endregion

	//Shake
	UPROPERTY(VisibleAnywhere, Category = "Camera Stats | State")
		TEnumAsByte<ECameraShakeState> shakeState;

	float shakeInTimer;
	float shakeOutTimer;
	float shakeOffset;
	float shakeAmp;
	float shakeSideAmp;
	float shakeFreq;
	float shakeStartOffset;
	float shakeSideStartOffset;
	float shakeHeight;
	float shakeSide;
	float prevHeight;
	bool blendingIn;
	bool blendingOut;
	bool bHitTrough;

	//FOV
	UPROPERTY(VisibleAnywhere, Category = "Camera Stats | State")
		TEnumAsByte<ECameraFOVState> fovState;
	TEnumAsByte<ECameraFOVState> prevFOVState;
	float fovTimer;
	bool fovTransition;
};
