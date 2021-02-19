#include "RunSlideState.h"
#include "../Player/PlayerPawn.h"

URunSlideState::URunSlideState() { }
URunSlideState::~URunSlideState() { }

void URunSlideState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Running Slide";
}

void URunSlideState::OnStateEnter()
{
	player->state = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->runSlideMat);
	player->playerCollider->AddForce(player->playerCollider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 1000); //multiply by 10k to keep designer values small
	AdjustCameraAndColliderPosition(player->crouchSlidePlayerHeight, player->crouchSlideCameraHeight);
}

void URunSlideState::StateTick(float DeltaTime)
{
	CheckIfSlideComplete();
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce);
	PlayerLook();
	ClampPlayerVelocity(player->runSlideMaxSpeed);
}

void URunSlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	AdjustCameraAndColliderPosition(player->standingPlayerHeight, player->standingCameraHeight);
}

//void URunSlideState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void URunSlideState::PlayerLook() { UMovementState::PlayerLook(); }
void URunSlideState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void URunSlideState::ClampPlayerVelocity(float max) { UMovementState::ClampPlayerVelocity(max); }
void URunSlideState::HandleJump(float jumpForce) { UMovementState::HandleJump(jumpForce); }
FVector URunSlideState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }


void URunSlideState::CheckIfSlideComplete() 
{
	//Passing to crouch so we don't have to reimplement the logic for standing up in enclosed spaces rules
	if (!player->bIsGrounded || player->playerCollider->GetPhysicsLinearVelocity().Size() <= player->runSlideExitVelocity)
	{
		player->stateMachine->SetState(player->stateMachine->crouchState);
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