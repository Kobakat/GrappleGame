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
	this->crouchTimer = 0;
	this->camTimer = 0;
	UState::Initialize(pawn);
}

void USlideState::OnStateEnter()
{
	player->stateName = this->stateName;
	player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
	crouchTimer = 0;
	camTimer = 0;
	bIsTransitioning = true;
}

void USlideState::StateTick(float deltaTime)
{
	CheckIfGrounded();
	HandleCrouchDown(deltaTime);
	HandleCameraTransition(deltaTime);
	HandleJump(player->slideJumpForce);

	if (!bIsTransitioning)
	{
		PlayerMove(player->slideAcceleration, 0);
		PlayerLook(deltaTime);
	}

	ClampPlayerVelocity(player->slideMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void USlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	crouchTimer = 0;
	camTimer = 0;
	player->bNeedsToStand = true;
}

#pragma endregion

#pragma region Game Logic
void USlideState::PlayerMove(float accel, float airControlFactor)
{
	if (!player->moveVector.IsZero())
	{
		FVector rightSlideVector = slideNormal.RotateAngleAxis(90, FVector::UpVector);
		FVector relativeVector = rightSlideVector * player->moveVector.X;
		relativeVector.Normalize(0.001);

		player->playerCollider->AddForce(relativeVector * accel, NAME_None, true); //Set to false if you want player mass to matter
	}
}

void USlideState::CheckIfGrounded() 
{
	FVector playerBottomLocation = FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());
	FVector rayOrigin = player->playerCollider->GetRelativeLocation() - playerBottomLocation;
	FVector rayDest = rayOrigin + (FVector::DownVector * player->slideGroundCheckOverride);
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	bool bHitGround = player->GetWorld()->LineTraceSingleByChannel(player->GroundHitPoint, rayOrigin, rayDest, ECC_Visibility, param);

	if (bHitGround)
	{
		FName struckProfile = player->GroundHitPoint.Component->GetCollisionProfileName();

		if (struckProfile != FName(TEXT("Slide")))
		{
			player->SetState(UCrouchState::GetInstance());		
		}

		else
		{
			slideNormal = player->GroundHitPoint.Normal;
			slideNormal.Z = 0;
			slideNormal.Normalize(0.001);
		}
	}

	else 
	{
		player->SetState(UCrouchState::GetInstance());
		player->bIsGrounded = false;
	}
}

void USlideState::HandleCrouchDown(float deltaTime)
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

void USlideState::HandleCameraTransition(float deltaTime)
{
	if (bIsTransitioning)
	{
		float frac = camTimer / player->camSlideTransitionTime;
		FRotator newCamRotator = FMath::Lerp(player->playerCamera->GetRelativeRotation(), slideNormal.Rotation(), frac);
		player->playerCamera->SetRelativeRotation(newCamRotator);

		camTimer += deltaTime;
		
		if (frac >= 1)
			bIsTransitioning = false;
	}
}

#pragma endregion