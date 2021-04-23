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
	bIsCrouching = true;
	crouchTimer = player->collider->GetCrouchTime(player->crouchHeight);
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
	player->collider->bNeedsToStand = true;
	player->collider->standUpTimer = player->collider->GetCrouchTime(player->standHeight);
}

#pragma endregion

#pragma region Game Logic

void UCrouchState::CheckIfPlayerIsTryingToStand() 
{
	if (!bIsCrouching && !player->tryingToCrouch && !player->collider->CheckIfTryingToStand() )
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
		//Only handle crouch if the player isn't already crouched down
		if (player->collider->halfHeight != player->crouchHeight) 
		{
			crouchTimer -= deltaTime;

			float frac = FMath::Clamp((crouchTimer / player->crouchTransitionTime), 0.f, 1.f);
			frac = frac * frac * (3.f - 2.f * frac);

			//Interoplate the collider/camera height
			const float newHeight = FMath::Lerp(player->crouchHeight, player->standHeight, frac);
			player->camera->baseHeight = FMath::Lerp(30, 60, frac); //HACK delete hard coded cam values

			//Set new values
			player->collider->SetCapsuleHalfHeight(newHeight);
			player->camera->SetRelativeLocation(FVector::UpVector * player->camera->baseHeight);

			const FVector capsuleLoc = player->collider->GetComponentLocation();
			const FVector base = FVector(
				capsuleLoc.X, 
				capsuleLoc.Y,
				capsuleLoc.Z - player->collider->halfHeight);

			player->collider->halfHeight = player->collider->GetScaledCapsuleHalfHeight();

			const FVector newLoc = base + (FVector::UpVector * player->collider->halfHeight);
			player->collider->SetRelativeLocation(newLoc);
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

