#include "RunSlideState.h"
#include "../Player/PlayerPawn.h"

URunSlideState::URunSlideState() { }
URunSlideState::~URunSlideState() { }
URunSlideState* URunSlideState::instance;
URunSlideState* URunSlideState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<URunSlideState>();
	}
	return instance;
}

#pragma region State Events
void URunSlideState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Running Slide";
	UState::Initialize(pawn);
}

void URunSlideState::OnStateEnter()
{
	player->stateName = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->runSlideMat);
	player->playerCollider->AddForce(player->playerCollider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 1000); //multiply by 10k to keep designer values small
	AdjustCameraAndColliderPosition(player->crouchSlidePlayerHeight, player->crouchSlideCameraHeight);
}

void URunSlideState::StateTick(float deltaTime)
{
	CheckIfSlideComplete();
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->runSlideMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void URunSlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	AdjustCameraAndColliderPosition(player->standingPlayerHeight, player->standingCameraHeight);
}

#pragma endregion

#pragma region Game Logic

void URunSlideState::CheckIfSlideComplete() 
{
	//Passing to crouch so we don't have to reimplement the logic for standing up in enclosed spaces rules
	if (!player->bIsGrounded || player->playerCollider->GetPhysicsLinearVelocity().Size() <= player->runSlideExitVelocity)
	{
		player->SetState(UCrouchState::GetInstance());
	}
}

void URunSlideState::AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight)
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