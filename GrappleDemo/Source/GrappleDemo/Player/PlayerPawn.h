#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShake.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../State/StateMachine.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "PlayerPawn.generated.h"

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
	Started,
	Shaking,
	Finishing
};

UCLASS()
class GRAPPLEDEMO_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	
	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	AGrappleReactor* grappleReactor;
	UStateMachine* stateMachine;

	UState* state;
	void SetState(UState* state);
	
	FHitResult GrappleHitPoint;
	FHitResult GroundHitPoint;
	bool bNeedsToStand;
	bool grappleCanAttach;

#pragma region Designer Props

	//=================Camera=================//

	UPROPERTY(EditAnywhere, Category = "Camera | General")
		UCameraComponent* playerCamera;

	//How many seconds does it take to pan the camera when the player steps on a slide
	UPROPERTY(EditAnywhere, Category = "Camera | General")
		float camSlideTransitionTime;

	//When the player is moving faster than this, increase the FOV
	UPROPERTY(EditAnywhere, Category = "Camera | FOV")
		float FOVVelocityThreshold;

	//This FOV is used while walking, crouching, and idling
	UPROPERTY(EditAnywhere, Category = "Camera | FOV")
		float FOVPassive;

	//This FOV is used while running, sliding, and swinging
	UPROPERTY(EditAnywhere, Category = "Camera | FOV")
		float FOVActive;

	//How many seconds does it take to adjust the camera FOV when switching to/from active or passive states
	UPROPERTY(EditAnywhere, Category = "Camera | FOV")
		float FOVTransitionTime;

	//Duration of the blend-in, where the oscillation scales from 0 to 1.
	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float shakeBlendInTime;

	//Duration of the blend-out, where the oscillation scales from 1 to 0.
	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float shakeBlendOutTime;

	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float passiveAmplitude;

	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float passiveFrequency;

	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float activeAmplitude;

	UPROPERTY(EditAnywhere, Category = "Camera | Shake", meta = (ClampMin = "0.0"))
		float activeFrequency;

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ECameraFOVState> fovState;
	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<ECameraShakeState> shakeState;


	TEnumAsByte<ECameraFOVState> prevFOVState;
	TEnumAsByte<ECameraShakeState> prevShakeState;

	float shakeTimer;
	float shakeOffset;
	bool shakeTransition;

	float fovTimer;
	bool stateTransition;

	//================Collider================//

	UPROPERTY(EditAnywhere, Category = "Collider")
		UCapsuleComponent* playerCollider;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* moveMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* stopMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* runSlideMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* frictionlessMat;

	//================General=================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float airborneMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float maxFallSpeed;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | General")
		float groundCheckDistance = 5;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float lookSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float standingPlayerHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float crouchSlidePlayerHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float standingCameraHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float crouchSlideCameraHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		FVector2D viewLookBounds;

	//=================Grapple================//
	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* grappleStart;
	UPROPERTY(EditAnywhere, Category = "Grapple")
		UGrappleComponent* grappleComponent;
	//=================Walking================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Walking")
		float walkAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Walking")
		float walkMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Walking")
		float walkJumpForce;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Walking")
		float walkAirControlPercentage;

	//=================Running================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Running")
		float runAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running")
		float runMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running")
		float runJumpForce;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running")
		float runAirControlPercentage;

	//=================Crouching================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		float crouchAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		float crouchMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		float crouchJumpForce;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		float crouchAirControlPercentage;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		float crouchTransitionTime;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Crouching")
		bool bCanPlayerCrouchJump;

	//How many extra units do we cast the ground checking ray?
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Crouching")
		float crouchGroundCheckOverride = 10;


	//===============Running=Slide==============//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideJumpForce;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideImpulse;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideExitVelocity;

	//How many extra units do we cast the ground checking ray?
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Running Slide")
		float runSlideGroundCheckOverride = 100;


	//===================Slide=================//
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideJumpForce;

	//How many extra units do we cast the ground checking ray?
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Sliding")
		float slideGroundCheckOverride = 100;

	//==============Instant=Reel===============//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float instantGrappleSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float reelCompleteDistance;

	//===================State=================//

	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		FString stateName;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		bool bIsGrounded;
		
#pragma endregion
	
#pragma region Input State

	FVector2D moveVector;
	FVector2D lookVector;
	float reelingAxis;
	bool tryingToSprint;
	bool tryingToJump;
	bool tryingToCrouch;
	bool tryingToGrapple;
	bool tryingToInstantReel;

#pragma endregion

protected:
	virtual void BeginPlay() override;

private:
	void HandleStandUp(float deltaTime);
	float standUpTimer;

#pragma region Input Functions
	void MoveInputX(float value);
	void MoveInputY(float value);
	void LookInputX(float value);
	void LookInputY(float value);
	void ReelInputAxis(float value);
	void JumpPress();
	void JumpRelease();
	void RunPress();
	void RunRelease();
	void CrouchSlidePress();
	void CrouchSlideRelease();
	void ShootReleasePress();
	void ShootReleaseRelease();
	void InstantReelPress();
#pragma endregion

#pragma region Grapple Functions
	bool CastGrappleRaycast();
	bool ShootGrapple();
#pragma endregion

#pragma region Camera Functions
	void UpdateCameraFOVState();
	void UpdateCameraFOV(float deltaTime);
	void UpdateCameraShakeState(float deltaTime);
	void StartCameraShake(float deltaTime, float camHeight, float amplitude, float freq);
	void EndCameraShake(float deltaTime, float camHeight, float amplitude, float freq);
	void UpdateCameraShake(float deltaTime, float camHeight, float amplitude, float freq);
#pragma endregion
};


