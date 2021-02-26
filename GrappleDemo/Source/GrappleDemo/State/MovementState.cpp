#include "MovementState.h"
#include "../Player/PlayerPawn.h"

UMovementState::~UMovementState() { }
void UMovementState::Initialize(APlayerPawn* pawn) { UState::Initialize(pawn); }
void UMovementState::OnStateEnter() { }
void UMovementState::StateTick(float DeltaTime) { }
void UMovementState::OnStateExit() { }

#pragma region Game Logic

void UMovementState::PlayerMove(float accel, float airControlFactor) 
{
	if (!player->moveVector.IsZero())
	{
		FVector relativeInputVector = ConvertPlayerInputRelativeToCamera();

		if (!player->bIsGrounded) 
		{
			relativeInputVector = relativeInputVector * (airControlFactor / 100.f);
		}

		//multiplying by 100 so the designer values aren't so massive
		player->playerCollider->AddForce(relativeInputVector * accel, NAME_None, true); //Set to false if you want player mass to matter
	}

	else if (
		player->moveVector.IsZero() 
		&& player->bIsGrounded
		&& player->state != UCrouchState::GetInstance()
		&& player->state != UGrappleAirborneState::GetInstance()
		&& player->state != UIdleState::GetInstance())
	{	
		player->SetState(UIdleState::GetInstance());	
	}
}

void UMovementState::PlayerLook(float deltaTime)
{
	if (!player->lookVector.IsZero())
	{
		FRotator camRotation = player->playerCamera->GetRelativeRotation();

		camRotation.Yaw += (player->lookVector.X * player->lookSpeed * player->GetWorld()->GetDeltaSeconds());
		camRotation.Pitch = FMath::Clamp(camRotation.Pitch + (player->lookVector.Y * player->lookSpeed * deltaTime), player->viewLookBounds.X, player->viewLookBounds.Y);

		player->playerCamera->SetRelativeRotation(camRotation);
	}
}

void UMovementState::CheckIfGrounded()
{
	FVector playerBottomLocation = FVector(0, 0, player->playerCollider->GetScaledCapsuleHalfHeight());
	FVector rayOrigin = player->playerCollider->GetRelativeLocation() - playerBottomLocation;
	FVector rayDest = rayOrigin + (FVector::DownVector * player->groundCheckDistance);
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	bool bHitGround = player->GetWorld()->LineTraceSingleByChannel(player->GroundHitPoint, rayOrigin, rayDest, ECC_Visibility, param);

	if (bHitGround)
	{
		FName struckProfile = player->GroundHitPoint.Component->GetCollisionProfileName();

		if (struckProfile == FName(TEXT("Ground")))
		{
			player->playerCollider->SetPhysMaterialOverride(player->moveMat);
			player->bIsGrounded = true;
		}

		else if (struckProfile == FName(TEXT("Slide")))
		{
			player->SetState(USlideState::GetInstance());
			player->bIsGrounded = true;
		}

		else
		{
			player->bIsGrounded = false;
			player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
		}
	}

	else
	{
		player->bIsGrounded = false;
		player->playerCollider->SetPhysMaterialOverride(player->frictionlessMat);
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
		player->tryingToJump = false;
		//multiply by 100 so designer values aren't so high
		player->playerCollider->SetPhysicsLinearVelocity(player->playerCollider->GetPhysicsLinearVelocity() + (FVector::UpVector * jumpForce));
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

void UMovementState::CheckStateChangeGrapple()
{
	if (player->tryingToGrapple)
	{
		player->SetState(UGrappleAirborneState::GetInstance());
	}
}

#pragma endregion