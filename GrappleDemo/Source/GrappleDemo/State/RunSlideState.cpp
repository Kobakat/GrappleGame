#include "RunSlideState.h"
#include "../Player/PlayerPawn.h"

URunSlideState::URunSlideState() { }
URunSlideState::~URunSlideState() { }
URunSlideState* URunSlideState::instance;
URunSlideState* URunSlideState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<URunSlideState>();
	}
	return instance;
}

#pragma region State Events
void URunSlideState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Running Slide";
	this->crouchTimer = 0;
	UState::Initialize(pawn);
}

void URunSlideState::OnStateEnter()
{
	player->stateName = this->stateName;
	player->collider->SetPhysMaterialOverride(player->collider->runSlideMat);
	player->collider->AddForce(player->collider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 1000); //multiply by 1k to keep designer values small
	
	bIsCrouching = true;
	crouchTimer = 0;
	player->collider->bNeedsToStand = false;
}

void URunSlideState::StateTick(float deltaTime)
{
	HandleCrouchDown(deltaTime);
	CheckIfSlideComplete();
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce, false);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->runSlideMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void URunSlideState::OnStateExit()
{
	player->collider->SetPhysMaterialOverride(player->collider->moveMat);
	bIsCrouching = false;
	crouchTimer = 0;
}

#pragma endregion

#pragma region Game Logic
void URunSlideState::CheckIfSlideComplete() 
{
	if (!bIsCrouching)
	{
		//Passing to crouch so we don't have to reimplement the logic for standing up in enclosed spaces rules
		if (!player->bGrounded || player->collider->GetPhysicsLinearVelocity().Size() <= player->runSlideExitVelocity)
		{
			player->SetState(UCrouchState::GetInstance());
		}
	}	
}

void URunSlideState::HandleCrouchDown(float deltaTime)
{
	if (bIsCrouching)
	{
		const float currentScale = player->collider->GetRelativeScale3D().Z;
		player->gun->SetRelativeScale3D(FVector(1, 1, 1.f / currentScale));
		//Only handle crouch if the player isn't already crouched down
		if (currentScale != player->crouchHeightScale)
		{
			crouchTimer += deltaTime;

			const float frac = FMath::Clamp((crouchTimer / player->crouchTransitionTime), 0.f, 1.f);
			const float newScale = FMath::Lerp(currentScale, player->crouchHeightScale, frac);

			player->collider->SetRelativeScale3D(FVector(1, 1, newScale));
		}

		else
		{
			bIsCrouching = false;
		}
	}
}

#pragma endregion