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
		relativeMovementVector = ConvertPlayerInputRelativeToCamera();

		if (player->bGrounded) 
		{
			player->collider->AddForce(relativeMovementVector * accel, NAME_None, true);
		}

		else 
		{		
			player->collider->AddForce(relativeMovementVector * (airControlFactor / 100.f) * accel, NAME_None, true);
		}
	}

	else if (
		player->moveVector.IsZero() 
		&& player->bGrounded
		&& player->state != UCrouchState::GetInstance()
		&& player->state != UGrappleAirborneState::GetInstance()
		&& player->state != UGrappleInstantReelState::GetInstance()
		&& player->state != UIdleState::GetInstance())
	{	
		player->SetState(UIdleState::GetInstance());	
	}
}

void UMovementState::PlayerLook(float deltaTime)
{
	if (!player->lookVector.IsZero())
	{
		FRotator camRotation = player->camera->GetRelativeRotation();

		camRotation.Yaw += (player->lookVector.X * player->camera->lookSpeed * player->GetWorld()->GetDeltaSeconds());
		camRotation.Pitch = FMath::Clamp(
			camRotation.Pitch + (player->lookVector.Y * player->camera->lookSpeed * deltaTime), 
			player->camera->viewLookBounds.X, 
			player->camera->viewLookBounds.Y);

		player->camera->SetRelativeRotation(camRotation);
	}
}

void UMovementState::CheckIfGrounded()
{
	player->bGrounded = player->collider->CheckIfGrounded();

	if (player->collider->bOnSlide && !player->collider->bNeedsToStand)
		player->SetState(USlideState::GetInstance());

	if (player->collider->GetPhysicsLinearVelocity().Z < 0.F &&
		player->bGrounded)
	{
		FVector velocity = player->collider->GetPhysicsLinearVelocity();
		player->collider->SetPhysicsLinearVelocity(FVector(velocity.X, velocity.Y, 0));
	}
	
	else
	{
		float newZ = 0;

		if (player->collider->CheckIfStepUp(newZ))
		{
			FVector velocity = player->collider->GetPhysicsLinearVelocity();
			FVector loc = player->collider->GetRelativeLocation();
			player->collider->SetRelativeLocation(FVector(loc.X + velocity.X * 0.01f, loc.Y + velocity.Y * 0.01f, newZ + .1f)); //HACK replace with deltaTime step
			player->collider->SetPhysicsLinearVelocity(player->collider->previousVelocity);
		}
	}
}

void UMovementState::ClampPlayerVelocity(float max)
{
	FVector movementVelocity = player->collider->GetPhysicsLinearVelocity();
	movementVelocity.Z = 0;

	//If the player is airborne we probably don't want to clamp speed
	if (player->bGrounded)
		movementVelocity = movementVelocity.GetClampedToMaxSize(max);
	else
		movementVelocity = movementVelocity.GetClampedToMaxSize(player->airborneMaxSpeed);

	FVector fallVelocity = player->collider->GetPhysicsLinearVelocity();
	fallVelocity.X = 0;
	fallVelocity.Y = 0;
	fallVelocity = fallVelocity.GetClampedToMaxSize(player->maxFallSpeed);

	FVector playerVelocity = movementVelocity + fallVelocity;

	player->collider->SetPhysicsLinearVelocity(playerVelocity);
}

void UMovementState::HandleJump(float jumpForce, bool bCanPlayerLedgeGrab) 
{
	if (!player->bGrounded)
	{
		FVector relativeMoveVector = ConvertPlayerInputRelativeToCamera();

		if (bCanPlayerLedgeGrab && player->collider->CheckIfLedgeGrabEligible(relativeMovementVector))
		{
			player->tryingToJump = false;
			player->SetState(ULedgeGrabState::GetInstance());
		}
	}

	if (player->tryingToJump && player->bGrounded)
	{
		player->tryingToJump = false;
		player->collider->SetPhysicsLinearVelocity(player->collider->GetPhysicsLinearVelocity() + (FVector::UpVector * jumpForce));

		if (bCanPlayerLedgeGrab && player->collider->CheckIfLedgeGrabEligible())
		{
			player->SetState(ULedgeGrabState::GetInstance());
		}
	}	
}

FVector UMovementState::ConvertPlayerInputRelativeToCamera()
{
	//Kill Z component and normalize vectors
	FVector camForwardVector = player->camera->GetForwardVector();
	FVector camRightVector = player->camera->GetRightVector();
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