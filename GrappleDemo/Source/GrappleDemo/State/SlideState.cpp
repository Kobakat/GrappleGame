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
	crouchTimer = 0;
	camTimer = 0;
	bIsTransitioning = true;
	bIsCrouching = true;
	player->bNeedsToStand = false;
	player->collider->SetPhysMaterialOverride(player->noFricMat);
}

void USlideState::StateTick(float deltaTime)
{
	CheckIfGrounded(player->slideGroundCheckOverride);
	HandleCrouchDown(deltaTime);
	HandleCameraTransition(deltaTime);
	HandleJump(player->slideJumpForce, false);

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
	crouchTimer = 0;
	camTimer = 0;
	player->bNeedsToStand = true;
	player->collider->SetPhysMaterialOverride(player->noFricMat);
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
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	float radius = player->bounds.X * .95f;

	FCollisionShape cap = FCollisionShape::MakeSphere(radius);

#if WITH_EDITOR

	DrawDebugSphere(
		player->GetWorld(),
		player->GetActorLocation(),
		radius,
		5,
		FColor::Red,
		false,
		0.05f);
#endif

	bool bHitSlide= player->GetWorld()->SweepSingleByChannel(
		player->GroundHitPoint,
		player->GetActorLocation() + (FVector::DownVector * overrideHeight),
		player->GetActorLocation() + (FVector::DownVector * overrideHeight),
		FQuat::Identity,
		ECC_Visibility,
		cap,
		param);

	if (bHitSlide)
	{
		FName struckProfile = player->GroundHitPoint.Component->GetCollisionProfileName();

		if (struckProfile == FName(TEXT("Slide")))
		{
			slideNormal = player->GroundHitPoint.Normal;
			slideNormal.Z = 0;
			slideNormal.Normalize(0.001);
			player->bIsGrounded = true;
		}
		
		else
		{
			player->SetState(UCrouchState::GetInstance());
			player->bIsGrounded = true;
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
	if (bIsCrouching)
	{
		float currentScale = player->collider->GetRelativeScale3D().Z;
		player->gun->SetRelativeScale3D(FVector(1, 1, 1.f / currentScale));
		//Only handle crouch if the player isn't already crouched down
		if (currentScale > player->crouchHeightScale)
		{
			crouchTimer += deltaTime;

			float frac = FMath::Clamp((crouchTimer / player->crouchTransitionTime), 0.f, 1.f);
			float newScale = FMath::Lerp(currentScale, player->crouchHeightScale, frac);

			player->collider->SetRelativeScale3D(FVector(1, 1, newScale));
		}

		else
		{
			bIsCrouching = false;
		}
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