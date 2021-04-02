#include "SlideState.h"
#include "../Player/PlayerPawn.h"

USlideState::USlideState() { }
USlideState::~USlideState() { }
USlideState* USlideState::instance;
FHitResult USlideState::slide;

USlideState* USlideState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<USlideState>();
	}
	return instance;
}

void USlideState::SetSlide(FHitResult newSlide)
{
	slide = newSlide;
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
	crouchTimer = player->collider->GetCrouchTime(player->crouchHeight);
	player->collider->SetPhysMaterialOverride(player->collider->noFricMat);
	player->collider->SetPhysicsLinearVelocity(player->collider->previousVelocity);
	bIsTransitioning = true;
	bIsCrouching = true;	
}

void USlideState::StateTick(float deltaTime)
{
	CheckIfGrounded();
	HandleCrouchDown(deltaTime);
	HandleJump(player->slideJumpForce, false);
	PlayerMove(player->slideAcceleration, 0);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->slideMaxSpeed);
	UMovementState::CheckStateChangeGrapple();
}

void USlideState::OnStateExit()
{
	player->collider->SetPhysMaterialOverride(player->collider->moveMat);
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

void USlideState::CheckIfGrounded()
{
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	const float radius = player->collider->bounds.X * .99f;
	const FVector capsuleLoc = player->collider->GetComponentLocation();
	const FVector sweepStart = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - player->collider->halfHeight + player->collider->bounds.X) + FVector::UpVector;
	const FVector sweepEnd = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - (player->collider->halfHeight * 2.f));
	const FCollisionShape sphere = FCollisionShape::MakeSphere(radius);

	const bool bHitSlide = player->GetWorld()->SweepMultiByChannel(
		player->collider->GroundHits,
		sweepStart,
		sweepEnd,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		sphere,
		param);

	if (bHitSlide)
	{
		bool onSlide = false;

		for (FHitResult hit : player->collider->GroundHits)
		{
			FName struckProfile = hit.Component->GetCollisionProfileName();

			if (struckProfile == FName(TEXT("Slide")))
			{
				slideNormal = hit.ImpactNormal;
				slideNormal.Z = 0;
				slideNormal.Normalize(0.001);
				player->collider->bOnSlide = true;
				player->bGrounded = true;
				return;
			}

			else
			{
				//HACK do a better check for a normal that is orthogonal to the Z plane
				if (hit.ImpactNormal.Z > .8f)
				{					
					break;
				}		
			}
		}
	}

	player->SetState(UCrouchState::GetInstance());
	player->bGrounded = true;
	player->collider->bOnSlide = false;	
}

void USlideState::HandleCrouchDown(float deltaTime)
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

void USlideState::HandleCameraTransition(float deltaTime)
{
	/*if (bIsTransitioning)
	{
		float frac = camTimer / player->camera->slideTransitionTime;
		FRotator newCamRotator = FMath::Lerp(player->camera->GetRelativeRotation(), slideNormal.Rotation(), frac);
		player->camera->SetRelativeRotation(newCamRotator);

		camTimer += deltaTime;
		
		if (frac >= 1)
			bIsTransitioning = false;
	}*/
}

#pragma endregion