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
	player->collider->SetPhysMaterialOverride(player->frictionlessMat);
	crouchTimer = 0;
	camTimer = 0;
	bIsTransitioning = true;
	player->bNeedsToStand = false;
}

void USlideState::StateTick(float deltaTime)
{
	CheckIfGrounded(player->slideGroundCheckOverride);
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
	player->collider->SetPhysMaterialOverride(player->moveMat);
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

		player->collider->AddForce(relativeVector * accel, NAME_None, true); //Set to false if you want player mass to matter
	}
}

void USlideState::CheckIfGrounded(float overrideHeight)
{
	FVector rayOrigin = player->collider->GetRelativeLocation();
	FVector rayDest = rayOrigin + (FVector::DownVector * overrideHeight);
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	bool bHitSlide = player->GetWorld()->LineTraceSingleByChannel(player->GroundHitPoint, rayOrigin, rayDest, ECC_GameTraceChannel2, param);

	if (bHitSlide)
	{
		slideNormal = player->GroundHitPoint.Normal;
		slideNormal.Z = 0;
		slideNormal.Normalize(0.001);
	}

	else
	{
		player->SetState(UCrouchState::GetInstance());
		player->bIsGrounded = false;
	}
}

void USlideState::HandleCrouchDown(float deltaTime)
{
	float currentScale = player->collider->GetRelativeScale3D().Z;
	//Only handle crouch if the player isn't already crouched down
	if (currentScale > player->crouchHeightScale)
	{
		crouchTimer += deltaTime;

		float frac = FMath::Clamp((crouchTimer / player->crouchTransitionTime), 0.f, 1.f);
		float newScale = FMath::Lerp(currentScale, player->crouchHeightScale, frac);

		player->collider->SetRelativeScale3D(FVector(1, 1, newScale));

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
		float frac = camTimer / player->camera->slideTransitionTime;
		FRotator newCamRotator = FMath::Lerp(player->camera->GetRelativeRotation(), slideNormal.Rotation(), frac);
		player->camera->SetRelativeRotation(newCamRotator);

		camTimer += deltaTime;
		
		if (frac >= 1)
			bIsTransitioning = false;
	}
}

#pragma endregion