#include "SlideState.h"
#include "../Player/PlayerPawn.h"

USlideState::USlideState() { }
USlideState::~USlideState() { }
USlideState* USlideState::instance;
USlideState* USlideState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<USlideState>();
	}
	return instance;
}

#pragma region State Events

void USlideState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Sliding";
	UState::Initialize(pawn);
}

void USlideState::OnStateEnter()
{
	player->stateName = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
	player->playerCollider->AddForce(player->playerCollider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 10000); //multiply by 10k to keep designer values small
	AdjustCameraAndColliderPosition(player->crouchSlidePlayerHeight, player->crouchSlideCameraHeight);
}

void USlideState::StateTick(float deltaTime)
{
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->runSlideMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void USlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	AdjustCameraAndColliderPosition(player->standingPlayerHeight, player->standingCameraHeight);
}

#pragma endregion

#pragma region Game Logic

void USlideState::CheckIfStillOnSlide() 
{
	if (!player->bIsGrounded) 
	{
		player->SetState(UWalkState::GetInstance());
	}
}

void USlideState::AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight)
{
	//Get the very bottom of the collider position
	FVector currentPos = player->playerCollider->GetRelativeLocation();
	currentPos = currentPos - FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());

	//Set the position of the player 
	//TODO lerp these values for smoother transition
	player->playerCollider->SetRelativeLocation(FVector(currentPos.X, currentPos.Y, currentPos.Z + capsuleHeight));
	player->playerCollider->SetCapsuleHalfHeight(capsuleHeight);

	player->playerCamera->SetRelativeLocation(FVector(0, 0, cameraHeight));
}

#pragma endregion