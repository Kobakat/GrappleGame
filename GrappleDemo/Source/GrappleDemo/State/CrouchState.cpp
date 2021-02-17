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
		
		//Cast a ray FROM the top of the crouched capsule height TO the top of the standing capsule height
		FVector currentPos = player->playerCollider->GetRelativeLocation();
		FVector rayOrigin = currentPos + (FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight()));
		FVector rayDest = currentPos - FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());
		rayDest = rayDest + (FVector(0, 0, player->standingPlayerHeight * 2));

		FCollisionQueryParams param;
		param.AddIgnoredActor(player);

		bool bHitCeiling = player->GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayDest, ECC_GameTraceChannel1, param);

		//if we don't hit anything they're good to stand up
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
	player->playerCollider->SetRelativeLocation(FVector(currentPos.X, currentPos.Y, currentPos.Z + capsuleHeight));
	player->playerCollider->SetCapsuleHalfHeight(capsuleHeight);

	player->playerCamera->SetRelativeLocation(FVector(0, 0, cameraHeight));
}

