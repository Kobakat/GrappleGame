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

	float radius = player->bounds.X * .95f;

	FCollisionShape cap = FCollisionShape::MakeSphere(radius);

#if WITH_EDITOR

	DrawDebugSphere(
		player->GetWorld(),
		player->GetActorLocation(),
		radius,
		10,
		FColor::Red, 
		false, 
		0.05f);
#endif

	bool bHitGround = player->GetWorld()->SweepSingleByChannel(
		player->GroundHitPoint,
		player->GetActorLocation(),
		player->GetActorLocation(),
		FQuat::Identity,
		ECC_Visibility,
		cap,
		param);

	if (bHitGround)
	{
		if (player->GetActorLocation().Z <= player->GroundHitPoint.ImpactPoint.Z /*+ overrideHeight*/)
		{
			FName struckProfile = player->GroundHitPoint.Component->GetCollisionProfileName();

			if (struckProfile == FName(TEXT("Ground")) || struckProfile == FName(TEXT("Ledge")))
			{
				player->bIsGrounded = true;

				if (player->bPreviousGround != player->bIsGrounded)
				{
					FVector velocity = player->collider->GetPhysicsLinearVelocity();
					player->collider->SetPhysicsLinearVelocity(FVector(velocity.X, velocity.Y, 0));
				}
				
			}

			else if (struckProfile == FName(TEXT("Slide")))
			{
				player->SetState(USlideState::GetInstance());
				player->bIsGrounded = true;
			}

			else
			{
				player->bIsGrounded = false;
			}
		}

		else
		{
			player->bIsGrounded = false;
		}
	}

	else
	{
		player->bIsGrounded = false;
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

void UMovementState::HandleJump(float jumpForce, bool bCanPlayerLedgeGrab) 
{
	if (player->tryingToJump && player->bIsGrounded)
	{
		player->tryingToJump = false;
		player->bIsGrounded = false;
		player->collider->SetPhysicsLinearVelocity(player->collider->GetPhysicsLinearVelocity() + (FVector::UpVector * jumpForce));
	}

	if (!player->bIsGrounded)
	{
		if (bCanPlayerLedgeGrab && CanPlayerLedgeGrab())
		{
			player->tryingToJump = false;
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

bool UMovementState::CanPlayerLedgeGrab() 
{
	//Start by sweeping our player region for a ledge

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FVector boundingBox = FVector(
		player->bounds.X * player->ledgeGrabRangeFactor, 
		player->bounds.Y * player->ledgeGrabRangeFactor, 
		player->bounds.Z);

	FCollisionShape box = FCollisionShape::MakeBox(boundingBox);

	bool bHitLedge = player->GetWorld()->SweepSingleByChannel(
		player->LedgeHitPoint,
		player->GetActorLocation() + FVector(0, 0, player->bounds.Z) + FVector::UpVector,
		player->GetActorLocation() + FVector(0, 0, player->bounds.Z) + FVector::UpVector,
		FQuat::Identity,
		ECC_GameTraceChannel6,
		box,
		param);

	//If we hit something in this layer
	if (bHitLedge)
	{
		//Lets make sure its exactly a ledge
		if (player->LedgeHitPoint.Component->GetCollisionProfileName() == FName(TEXT("Ledge")))
		{	
			//Can our player reach this high?
			FVector ledgeBounds = player->LedgeHitPoint.Component->Bounds.BoxExtent;
			ledgeBounds += player->LedgeHitPoint.Actor.Get()->GetActorLocation();

			if (ledgeBounds.Z - player->GetActorLocation().Z <= player->ledgeGrabHeight)
			{
				FVector camLoc = player->camera->GetComponentLocation();

				//Calculate the normal of the way our player is looking (without Z)
				FVector camForward = player->camera->GetForwardVector();
				camForward.Z = 0;
				camForward.Normalize(0.01f);

				//Calculate the opposite of the ledge's normal
				FVector impactNormal = player->LedgeHitPoint.ImpactNormal;
				impactNormal.Z = 0;
				impactNormal.Normalize(0.01f);
				impactNormal *= -1;

				//Calculate the Angle between the two vectors
				const float dot = FVector::DotProduct(camForward, impactNormal);
				const float angle = FMath::RadiansToDegrees(FMath::Acos(dot));

				//If the angle is small enough, the player is eligible to climb the ledge
				const bool lookingAt = angle <= player->ledgeLookAngle;

				if (lookingAt)
					return true;
			}
		}
	}
	return false;
}

void UMovementState::CheckStateChangeGrapple()
{
	if (player->tryingToGrapple)
	{
		player->SetState(UGrappleAirborneState::GetInstance());
	}
}

#pragma endregion