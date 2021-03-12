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
	player->bNeedsToStand = false;
	bIsCrouching = true;
	crouchTimer = 0;
}

void UCrouchState::StateTick(float deltaTime)
{
	HandleCrouchDown(deltaTime);
	CheckIfGrounded(player->crouchGroundCheckOverride);
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
	player->standUpTimer = 0;
}

#pragma endregion

#pragma region Game Logic

void UCrouchState::CheckIfPlayerIsTryingToStand() 
{
	if (!player->tryingToCrouch && !bIsCrouching) 
	{
		FCollisionQueryParams param;
		param.AddIgnoredActor(player);

		FCollisionShape box = FCollisionShape::MakeBox(player->bounds);

		bool bHitCeiling = player->GetWorld()->SweepSingleByChannel(
			player->CrouchHitPoint, 
			player->GetActorLocation() + FVector(0, 0, player->bounds.Z) + FVector::UpVector,
			player->GetActorLocation() + FVector(0, 0, player->bounds.Z) + FVector::UpVector,
			FQuat::Identity, 
			ECC_Visibility, 
			box,
			param);

#if WITH_EDITOR
		DrawDebugBox(player->GetWorld(), player->GetActorLocation() + FVector(0, 0, player->bounds.Z) + FVector::UpVector, player->bounds, FColor::Purple, false, .05f);
#endif

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

#pragma endregion

