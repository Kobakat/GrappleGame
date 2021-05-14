#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "cringetest.h"
#include "Components/InputComponent.h"
#include "../State/StateMachine.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "../GrappleRendering/PolylineCylinderRenderer.h"
#include "PlayerCapsule.h"
#include "GrappleGunComponent.h"
#include "CursorType.h"
#include "PlayerPawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLandAudio, float, landSpeed);

UCLASS()
class GRAPPLEDEMO_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadOnly, Category = "Grapple Gun")
	UGrappleGunComponent* grappleComponent;

	UStateMachine* stateMachine;
	UState* state;
	void SetState(UState* state);

	bool bPreviousGround;
	float standUpTimer;

	FVector bounds;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Camera")
		Ucringetest* camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collider")
		UPlayerCapsule* collider;
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
		bool grappleCanAttach;
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
		TEnumAsByte<ECursorType> Cursor;

	UPROPERTY(BlueprintAssignable)
		FLandAudio OnLand;
	UPROPERTY(BlueprintAssignable)
		FStateEvent OnJump;	
	UPROPERTY(BlueprintAssignable)
		FStateEvent OnLedgeClimb;
	UPROPERTY(BlueprintAssignable)
		FStateEvent OnSlideBegin;
	UPROPERTY(BlueprintAssignable)
		FStateEvent OnSlideEnd;

	float lastFallingSpeed;

#pragma region Designer Props

	//================General=================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float airborneMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float maxFallSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float standHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float crouchHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float stepHeight;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float maxSlopeAngle;
	//=================Grapple================//

	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* gun;

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

	//===============Running=Slide==============//
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideJumpForce;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideImpulse;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Running Slide")
		float runSlideExitVelocity;

	//===================Slide=================//
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideJumpForce;

	//==============Instant=Reel===============//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		bool isHookshotStyle;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float instantGrappleSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float reelCompleteDistance;

	//===============Ledge=Grab================//

	//How many units a second the player climbs
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeClimbSpeed;
	//How far forward to push the player when they lazy climb
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgePushSpeed;
	//What is the maximum angles (in degrees) our player can be looking at a ledge to climb it
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeLookAngle;
	//The maximum units up our player can grab (distance from bottom of player)
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeMaxGrabHeight;
	//The minimum units up our player can grab (distance from bottom of player)
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeMinGrabHeight;
	//How many player radius' to expand the ledge grab range by
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeGrabRangeFactor;

	//===================State=================//

	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		FString stateName;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		bool bGrounded;
	bool bPreviousGrounded;

	//===============Grapple=Wrap==============//

	APolylineCylinderRenderer* GrapplePolyline;
		
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

	FVector startLocation;

	// Gets whether the player can currently grapple
	UFUNCTION(BlueprintCallable)
	bool GetHasGrapple();
	// Sets whether the player can currently grapple
	UFUNCTION(BlueprintCallable)
	void SetHasGrapple(bool HasGrapple);

	UFUNCTION(BlueprintCallable)
	void BindPreferences();

protected:
	virtual void BeginPlay() override;

private:
	// These variables facilitate the
	// buffering of grapple input
	bool SwingBuffered;
	bool InstantBuffered;
	float BufferedTime;

	UPROPERTY(EditAnywhere, Category = "Initial Player State")
	bool hasGrapple;

	bool Linked;
	void LinkPreviewCamera();
	
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
};