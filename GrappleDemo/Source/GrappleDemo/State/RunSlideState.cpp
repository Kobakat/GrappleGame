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
	player->playerCollider->SetPhysMaterialOverride(player->runSlideMat);
	player->playerCollider->AddForce(player->playerCollider->GetPhysicsLinearVelocity().GetClampedToMaxSize(1) * player->runSlideImpulse * 1000); //multiply by 10k to keep designer values small
	crouchTimer = 0;
}

void URunSlideState::StateTick(float deltaTime)
{
	HandleCrouchDown(deltaTime);
	CheckIfSlideComplete();
	CheckIfGrounded();
	HandleJump(player->runSlideJumpForce);
	PlayerLook(deltaTime);
	ClampPlayerVelocity(player->runSlideMaxSpeed);

	UMovementState::CheckStateChangeGrapple();
}

void URunSlideState::OnStateExit()
{
	player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	crouchTimer = 0;
}

#pragma endregion

#pragma region Game Logic
void URunSlideState::CheckIfGrounded()
{
	FVector playerBottomLocation = FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());
	FVector rayOrigin = player->playerCollider->GetRelativeLocation() - playerBottomLocation;
	FVector rayDest = rayOrigin + (FVector::DownVector * player->runSlideGroundCheckOverride);
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	bool bHitGround = player->GetWorld()->LineTraceSingleByChannel(player->GroundHitPoint, rayOrigin, rayDest, ECC_Visibility, param);

	//CheckIfSlideCompletes handles the rest
	if(!bHitGround)
	{
		player->bIsGrounded = false;		
	}	
}

void URunSlideState::CheckIfSlideComplete() 
{
	if (!bIsCrouching) //this might f things up
	{
		//Passing to crouch so we don't have to reimplement the logic for standing up in enclosed spaces rules
		if (!player->bIsGrounded || player->playerCollider->GetPhysicsLinearVelocity().Size() <= player->runSlideExitVelocity)
		{
			player->SetState(UCrouchState::GetInstance());
		}
	}	
}

void URunSlideState::HandleCrouchDown(float deltaTime)
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