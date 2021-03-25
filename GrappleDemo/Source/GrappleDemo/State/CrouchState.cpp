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
	player->collider->bNeedsToStand = false;
	bIsCrouching = true;
	crouchTimer = 0;
}

void UCrouchState::StateTick(float deltaTime)
{
	HandleCrouchDown(deltaTime);
	CheckIfGrounded();
	CheckIfPlayerIsTryingToStand();
	HandleJump(player->crouchJumpForce, false);
	PlayerMove(player->crouchAcceleration, player->crouchAirControlPercentage);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->crouchMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void UCrouchState::OnStateExit()
{
	crouchTimer = 0;
	player->collider->bNeedsToStand = true;
	player->collider->standUpTimer = 0;
}

#pragma endregion

#pragma region Game Logic

void UCrouchState::CheckIfPlayerIsTryingToStand() 
{
	if (player->collider->CheckIfTryingToStand())
	{
		player->SetState(UWalkState::GetInstance());
	}
}

void UCrouchState::HandleJump(float jumpForce, bool bCanPlayerLedgeGrab)
{
	if (player->bCanPlayerCrouchJump) 
	{
		UMovementState::HandleJump(jumpForce, bCanPlayerLedgeGrab);
	}
}

void UCrouchState::HandleCrouchDown(float deltaTime)
{
	if (bIsCrouching)
	{
		const float currentScale = player->collider->GetRelativeScale3D().Z;
	
		player->gun->SetRelativeScale3D(FVector(1, 1, 1.f / currentScale));
		//Only handle crouch if the player isn't already crouched down
		if (currentScale > player->crouchHeightScale) 
		{
			crouchTimer += deltaTime;

			const float frac = FMath::Clamp((crouchTimer / player->crouchTransitionTime), 0.f, 1.f);
			const float newScale = FMath::Lerp(currentScale, player->crouchHeightScale, frac);

			player->collider->SetRelativeScale3D(FVector(1,1, newScale));
		}

		else 
		{
			bIsCrouching = false;		
		}
	}
}

void UCrouchState::PlayerMove(float accel, float airControlFactor)
{
	if (!player->moveVector.IsZero())
	{
		player->collider->SetPhysMaterialOverride(player->collider->moveMat);

		FVector relativeInputVector = ConvertPlayerInputRelativeToCamera();

		if (!player->bGrounded)
		{
			relativeInputVector = relativeInputVector * (airControlFactor / 100.f);
		}

		player->collider->AddForce(relativeInputVector * accel, NAME_None, true);
	}

	else if (player->moveVector.IsZero() && player->bGrounded)
	{
		player->collider->SetPhysMaterialOverride(player->collider->stopMat);
	}
}

#pragma endregion

