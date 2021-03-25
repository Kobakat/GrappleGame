#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "cringetest.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "../State/StateMachine.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "../GrappleRendering/PolylineCylinderRenderer.h"
#include "GrappleGunComponent.h"
#include "PlayerPawn.generated.h"

UCLASS()
class GRAPPLEDEMO_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	
	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UGrappleGunComponent* grappleComponent;
	UStateMachine* stateMachine;
	UState* state;
	void SetState(UState* state);
	
	FHitResult CrouchHitPoint;
	FHitResult GroundHitPoint;
	FHitResult LedgeHitPoint;
	bool bNeedsToStand;
	bool bPreviousGround;
	float standUpTimer;

	FVector bounds;

	UPROPERTY(VisibleAnywhere, Category = "Player Camera")
		Ucringetest* camera;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UStaticMeshComponent* collider;
	UPROPERTY(BlueprintReadOnly, Category = "Grapple")
		bool grappleCanAttach;

#pragma region Designer Props

	//================Collider================//
	
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* moveMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* stopMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* runSlideMat;
	UPROPERTY(EditAnywhere, Category = "Collider")
		UPhysicalMaterial* noFricMat;

	//================General=================//

	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float airborneMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float maxFallSpeed;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | General")
		float groundCheckDistance = 5;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float standHeightScale;
	UPROPERTY(EditAnywhere, Category = "Player Stats | General")
		float crouchHeightScale;
	
	//=================Grapple================//
	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* gun;
	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* grappleStart;
	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* grappleClawOrigin;
	UPROPERTY(BlueprintReadWrite, Category = "Grapple")
		USceneComponent* grappleClaw;
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
		float crouchGroundCheckOverride = 5;


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
		float runSlideGroundCheckOverride = 5;


	//===================Slide=================//
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideAcceleration;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideJumpForce;

	//How many extra units do we cast the ground checking ray?
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | Sliding")
		float slideGroundCheckOverride = 30;

	//==============Instant=Reel===============//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float instantGrappleSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float reelCompleteDistance;

	//===============Ledge=Grab================//
	//How many units does our player climb each second
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
		float ledgeGrabHeight;
	//How many player radius' to expand the ledge grab range by
	UPROPERTY(EditAnywhere, Category = "Player Stats | Ledge Grab")
		float ledgeGrabRangeFactor;

	//===================State=================//

	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		FString stateName;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		bool bIsGrounded;

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

protected:
	virtual void BeginPlay() override;

private:
	void HandleStandUp(float deltaTime);
	FVector CalculateBounds();

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