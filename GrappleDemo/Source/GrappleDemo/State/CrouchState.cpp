#include "CrouchState.h"
#include "../Player/PlayerPawn.h"

UCrouchState::UCrouchState() { }
UCrouchState::~UCrouchState() { }
UCrouchState* UCrouchState::instance;
UCrouchState* UCrouchState::GetInstance() 
{ 
	if (instance == nullptr) 
	{
		instance = NewObject<UCrouchState>();
	}
	return instance; 
}

#pragma region State Events

void UCrouchState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Crouching";
	this->crouchTimer = 0;
	UState::Initialize(pawn);	
}

void UCrouchState::OnStateEnter()
{
	player->stateName = this->stateName;
	crouchTimer = 0;
}

void UCrouchState::StateTick(float deltaTime)
{
	HandleCrouchDown(deltaTime);
	CheckIfGrounded();
	CheckIfPlayerIsTryingToStand();
	HandleJump(player->crouchJumpForce);
	PlayerMove(player->crouchAcceleration, player->crouchAirControlPercentage);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->crouchMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void UCrouchState::OnStateExit()
{
	crouchTimer = 0;
	player->bNeedsToStand = true;
}

#pragma endregion

#pragma region Game Logic
void UCrouchState::CheckIfPlayerIsTryingToStand() 
{
	if (!player->tryingToCrouch && !bIsCrouching) 
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
			player->SetState(UWalkState::GetInstance());
		}
	}
}

void UCrouchState::HandleJump(float jumpForce)
{
	if (player->bCanPlayerCrouchJump) 
	{
		UMovementState::HandleJump(jumpForce);
	}
}

void UCrouchState::HandleCrouchDown(float deltaTime)
{
	//Only handle crouch if the player isn't already crouched down
	if (player->playerCollider->GetScaledCapsuleHalfHeight() > player->crouchSlidePlayerHeight) 
	{
		float frac = crouchTimer / player->crouchTransitionTime;
		float newCapHeight = FMath::Lerp(player->standingPlayerHeight, player->crouchSlidePlayerHeight, frac);
		float newCamHeight = FMath::Lerp(player->standingCameraHeight, player->crouchSlideCameraHeight, frac);

		player->playerCollider->SetCapsuleHalfHeight(newCapHeight);
		player->playerCamera->SetRelativeLocation(FVector(0, 0, newCamHeight));

		crouchTimer += deltaTime;
		bIsCrouching = true;
	}

	else 
	{
		bIsCrouching = false;		
	}
}

#pragma endregion

