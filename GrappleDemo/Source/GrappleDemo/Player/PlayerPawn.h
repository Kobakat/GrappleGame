#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "../State/StateMachine.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "PlayerPawn.generated.h"

UCLASS()
class GRAPPLEDEMO_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();
	bool CastGrappleRaycast();
	bool tryingToInstantReel;

	virtual void Tick(float deltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UGrappleComponent* grappleComponent;
	AGrappleReactor* grappleReactor;

	UPROPERTY(BlueprintReadWrite, Category = "Grapple Parameters")
		USceneComponent* grappleStart;
	UPROPERTY()
		float raycastDistance;

#pragma region Designer Props
	//=================Camera=================//

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* playerCamera;

	float cameraTargetFOV;
	float averageVelocity;

	void UpdateCameraFOV();

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
		float slideMaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Sliding")
		float slideJumpForce;

	//==============Instant=Reel===============//

	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float instantGrappleSpeed;
	UPROPERTY(EditAnywhere, Category = "Player Stats | Instant Grapple")
		float reelCompleteDistance;

	//===================State=================//

	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		FString state;
	UPROPERTY(VisibleAnywhere, Category = "Player Stats | State")
		bool bIsGrounded;
	UPROPERTY() //garbage collector gets angry if this isn't a uprop?
		UStateMachine* stateMachine;
#pragma endregion
	
#pragma region Input State
	FVector2D moveVector;
	FVector2D lookVector;
	float reelingAxis;
	bool tryingToSprint;
	bool tryingToJump;
	bool tryingToCrouch;
	bool tryingToGrapple;
	// These inputs are consumed when observed.
	//bool IsTryingToGrapple();
	bool IsTryingToInstantReel();
#pragma endregion

private:
	bool grappleInputBuffered;
	bool instantReelInputBuffered;

protected:
	virtual void BeginPlay() override;

#pragma region Input Functions
	void MoveInputX(float value);
	void MoveInputY(float value);
	void LookInputX(float value);
	void LookInputY(float value);
	void JumpPress();
	void JumpRelease();
	void RunPress();
	void RunRelease();
	void CrouchSlidePress();
	void CrouchSlideRelease();
	void ReelInputAxis(float value);
	void ShootReleasePress();
	void ShootReleaseRelease();
	void InstantReelPress();
#pragma endregion
};


