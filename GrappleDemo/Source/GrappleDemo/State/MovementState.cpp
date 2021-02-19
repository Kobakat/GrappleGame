#include "MovementState.h"
#include "../Player/PlayerPawn.h"

UMovementState::~UMovementState() { }
void UMovementState::Initialize(APlayerPawn* pawn) { UState::Initialize(pawn); }
void UMovementState::OnStateEnter() { }
void UMovementState::StateTick() { }
void UMovementState::OnStateExit() { }

#pragma region Game Logic

void UMovementState::PlayerMove(float accel, float airControlFactor) 
{
	if (!player->moveVector.IsZero())
	{
		FVector relativeInputVector = ConvertPlayerInputRelativeToCamera();

		if (!player->bIsGrounded) {
			relativeInputVector * (airControlFactor / 100.f);
		}

		//multiplying by 100 so the designer values aren't so massive
		player->playerCollider->AddForce(relativeInputVector * accel * player->GetWorld()->GetDeltaSeconds() * 100.f, NAME_None, true); //Set to false if you want player mass to matter
	}

	else if (player->moveVector.IsZero() && player->bIsGrounded && player->stateMachine->state != player->stateMachine->crouchState) 
	{
		if (player->stateMachine->state != player->stateMachine->idleState) 
		{
			player->stateMachine->SetState(player->stateMachine->idleState);
		}
	}
}

void UMovementState::PlayerLook()
{
	if (!player->lookVector.IsZero())
	{
		FRotator camRotation = player->playerCamera->GetRelativeRotation();

		camRotation.Yaw += (player->lookVector.X * player->lookSpeed * player->GetWorld()->GetDeltaSeconds());
		camRotation.Pitch = FMath::Clamp(camRotation.Pitch + (player->lookVector.Y * player->lookSpeed * player->GetWorld()->GetDeltaSeconds()), player->viewLookBounds.X, player->viewLookBounds.Y);

		player->playerCamera->SetRelativeRotation(camRotation);
	}
}

void UMovementState::CheckIfGrounded()
{
	FHitResult hit;
	FVector playerBottomLocation = FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());
	FVector rayOrigin = player->playerCollider->GetRelativeLocation() - playerBottomLocation;
	FVector rayDest = rayOrigin + (FVector::DownVector);
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	bool bHitGround = player->GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayDest, ECC_GameTraceChannel1, param);

	if (!bHitGround) 
	{
		bool bHitSlide = player->GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayDest, ECC_GameTraceChannel2, param);
		
		if (bHitSlide && player->stateMachine->state != player->stateMachine->slideState) 
		{
			player->stateMachine->SetState(player->stateMachine->slideState);
			player->bIsGrounded = true;
		}

		else if (!bHitSlide) 
		{
			player->bIsGrounded = false;
			player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
		}
	}

	else 
	{
		player->bIsGrounded = true;
		player->playerCollider->SetPhysMaterialOverride(player->moveMat);
	}
}

void UMovementState::ClampPlayerVelocity(float max)
{
	FVector movementVelocity = player->playerCollider->GetPhysicsLinearVelocity();
	movementVelocity.Z = 0;

	//If the player is airborne we probably don't want to clamp speed
	if (player->bIsGrounded)
		movementVelocity = movementVelocity.GetClampedToMaxSize(max);
	else
		movementVelocity = movementVelocity.GetClampedToMaxSize(player->airborneMaxSpeed);

	FVector fallVelocity = player->playerCollider->GetPhysicsLinearVelocity();
	fallVelocity.X = 0;
	fallVelocity.Y = 0;
	fallVelocity = fallVelocity.GetClampedToMaxSize(player->maxFallSpeed);

	FVector playerVelocity = movementVelocity + fallVelocity;

	player->playerCollider->SetPhysicsLinearVelocity(playerVelocity);
}

void UMovementState::HandleJump(float jumpForce) 
{
	if (player->tryingToJump && player->bIsGrounded) 
	{
		//multiply by 100 so designer values aren't so high
		player->playerCollider->AddForce(FVector::UpVector * jumpForce * 100.f);
	}
}

FVector UMovementState::ConvertPlayerInputRelativeToCamera()
{
	//Kill Z component and normalize vectors
	FVector camForwardVector = player->playerCamera->GetForwardVector();
	FVector camRightVector = player->playerCamera->GetRightVector();
	camForwardVector.Z = 0;
	camRightVector.Z = 0;
	camForwardVector.Normalize(0.0001);
	camRightVector.Normalize(0.0001);

	//Multiply input axis values by the cameras look vectors
	FVector relativeVector = (camForwardVector * player->moveVector.Y) + (camRightVector * player->moveVector.X);
	relativeVector.Normalize(0.0001);

	return relativeVector;
}

#pragma endregion