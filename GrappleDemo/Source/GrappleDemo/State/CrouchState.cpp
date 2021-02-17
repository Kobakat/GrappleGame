#include "CrouchState.h"
#include "../Player/PlayerPawn.h"

UCrouchState::UCrouchState() { }
UCrouchState::~UCrouchState() { }

void UCrouchState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	this->stateName = "Crouching";
}

void UCrouchState::OnStateEnter()
{
	player->state = this->stateName;
	AdjustCameraAndColliderPosition(player->crouchSlidePlayerHeight, player->crouchSlideCameraHeight);
}

void UCrouchState::StateTick()
{
	CheckIfGrounded();
	CheckIfPlayerIsTryingToStand();
	HandleJump(player->crouchJumpForce);
	PlayerMove(player->crouchAcceleration, player->crouchAirControlPercentage);
	PlayerLook();
	ClampPlayerVelocity(player->crouchMaxSpeed);
}

void UCrouchState::OnStateExit()
{
	AdjustCameraAndColliderPosition(player->standingPlayerHeight, player->standingCameraHeight);
}

void UCrouchState::CheckIfPlayerIsTryingToStand() 
{
	if (!player->tryingToCrouch) 
	{
		FHitResult hit = FHitResult();
		
		FVector rayOrigin = player->playerCollider->GetRelativeLocation() + (FVector(0, 0, player->crouchSlidePlayerHeight) * 0.5f);
		FVector rayDest = player->playerCollider->GetRelativeLocation() + (FVector(0, 0, player->standingPlayerHeight) * 0.5f);
		FCollisionQueryParams param;
		param.AddIgnoredActor(player);

		bool bHitCeiling = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayDest, ECC_Visibility, param);

		if (!bHitCeiling) 
		{
			player->stateMachine->SetState(player->stateMachine->walkState);
		}
	}
}

void UCrouchState::PlayerMove(float accel, float airControlFactor) { UMovementState::PlayerMove(accel, airControlFactor); }
void UCrouchState::PlayerLook() { UMovementState::PlayerLook(); }
void UCrouchState::CheckIfGrounded() { UMovementState::CheckIfGrounded(); }
void UCrouchState::ClampPlayerVelocity(float max) { UMovementState::ClampPlayerVelocity(max); }
FVector UCrouchState::ConvertPlayerInputRelativeToCamera() { return UMovementState::ConvertPlayerInputRelativeToCamera(); }

void UCrouchState::HandleJump(float jumpForce)
{
	if (player->bCanPlayerCrouchJump) 
	{
		UMovementState::HandleJump(jumpForce);
	}
}

void UCrouchState::AdjustCameraAndColliderPosition(float capsuleHeight, float cameraHeight)
{
	//Get the very bottom of the collider position
	FVector currentPos = player->playerCollider->GetRelativeLocation();
	currentPos = currentPos - FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());

	//Set the position of the player 
	//TODO lerp these values for smoother transition
	player->playerCollider->SetRelativeLocation(FVector(currentPos.X, currentPos.Y, capsuleHeight));
	player->playerCollider->SetCapsuleHalfHeight(capsuleHeight);

	player->playerCamera->SetRelativeLocation(FVector(0, 0, cameraHeight));
}

