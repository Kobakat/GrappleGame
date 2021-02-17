#include "SlideState.h"
#include "../Player/PlayerPawn.h"

USlideState::USlideState() { }
USlideState::~USlideState() { }

void USlideState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Sliding";
}

void USlideState::OnStateEnter()
{
	player->state = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
	player->playerCollider->AddForce(player->playerCollider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 10000); //multiply by 10k to keep designer values small
	AdjustCameraAndColliderPosition(player->crouchSlidePlayerHeight, player->crouchSlideCameraHeight);
}

void USlideState::StateTick()
{
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce);
	PlayerLook();
	ClampPlayerVelocity(player->runSlideMaxSpeed);
}

void USlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	AdjustCameraAndColliderPosition(player->standingPlayerHeight, player->standingCameraHeight);
}

void URunSlideState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void USlideState::PlayerLook() { UMovementState::PlayerLook(); }
void USlideState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void USlideState::ClampPlayerVelocity(float max) { UMovementState::ClampPlayerVelocity(max); }
void USlideState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }
FVector USlideState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }

void USlideState::CheckIfStillOnSlide() 
{
	if (!player->bIsGrounded) 
	{
		player->stateMachine->SetState(player->stateMachine->walkState);
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