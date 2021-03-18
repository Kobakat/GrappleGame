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
		player->collider->AddForce(relativeInputVector * accel, NAME_None, true); //Set to false if you want player mass to matter
	}

	else if (
		player->moveVector.IsZero() 
		&& player->bIsGrounded
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

void UMovementState::CheckIfGrounded(float overrideHeight)
{
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FVector boxBounds = FVector(player->bounds.X, player->bounds.Y, overrideHeight);
	//shrink so they dont extend beyond the player collider and stay central
	boxBounds.X *= .75f;
	boxBounds.Y *= .75f;
	FCollisionShape box = FCollisionShape::MakeBox(boxBounds);

#if WITH_EDITOR
	DrawDebugBox(player->GetWorld(), player->GetActorLocation() + (FVector::DownVector * overrideHeight) + FVector::UpVector, boxBounds, FColor::Red, false, 0.05f);
#endif

	bool bHitGround = player->GetWorld()->SweepSingleByChannel(
		player->GroundHitPoint,
		player->GetActorLocation() + (FVector::DownVector * overrideHeight),
		player->GetActorLocation() + (FVector::DownVector * overrideHeight),
		FQuat::Identity,
		ECC_Visibility,
		box,
		param);

	if (bHitGround)
	{
		FName struckProfile = player->GroundHitPoint.Component->GetCollisionProfileName();

		if (struckProfile == FName(TEXT("Ground")))
		{
			player->collider->SetPhysMaterialOverride(player->moveMat);
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
			player->collider->SetPhysMaterialOverride(player->frictionlessMat);
		}
	}

	else
	{
		player->bIsGrounded = false;
		player->collider->SetPhysMaterialOverride(player->frictionlessMat);
	}
}

void UMovementState::ClampPlayerVelocity(float max)
{
	FVector movementVelocity = player->collider->GetPhysicsLinearVelocity();
	movementVelocity.Z = 0;

	//If the player is airborne we probably don't want to clamp speed
	if (player->bIsGrounded)
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

void UMovementState::HandleJump(float jumpForce) 
{
	if (player->tryingToJump && player->bIsGrounded) 
	{
		player->tryingToJump = false;
		//multiply by 100 so designer values aren't so high
		player->collider->SetPhysicsLinearVelocity(player->collider->GetPhysicsLinearVelocity() + (FVector::UpVector * jumpForce));
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