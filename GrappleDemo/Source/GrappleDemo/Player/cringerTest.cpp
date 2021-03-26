#include "cringerTest.h"
#include "PlayerPawn.h"

UcringerTest::UcringerTest() { PrimaryComponentTick.bCanEverTick = true; }

void UcringerTest::BeginPlay()
{
	player = Cast<APlayerPawn>(GetAttachmentRootActor());
	SetRelativeScale3D(FVector(1, 1, player->standHeightScale));
	bounds = CalculateBounds();
}

void UcringerTest::TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	HandleStandUp(deltaTime);
	previousVelocity = GetPhysicsLinearVelocity();
}


FVector UcringerTest::CalculateBounds()
{
	return GetStaticMesh()->GetBounds().BoxExtent;
}


void UcringerTest::HandleStandUp(float deltaTime)
{
	//TODO sweep by multi
	//iterate through list
	//Discard hits against surfaces with an orthogonal to Z-up normal
	//This will let player stand up when directly against a wall

	if (bNeedsToStand)
	{
		FCollisionQueryParams param;
		param.AddIgnoredActor(player);

		FCollisionShape box = FCollisionShape::MakeBox(bounds);

		bool bHitCeiling = GetWorld()->SweepSingleByChannel(
			CrouchHitPoint,
			GetComponentLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
			GetComponentLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
			FQuat::Identity,
			ECC_Visibility,
			box,
			param);

		if (bHitCeiling)
		{
			player->SetState(UCrouchState::GetInstance());
		}

		else
		{
			const float currentScale = GetRelativeScale3D().Z;
			player->gun->SetRelativeScale3D(FVector(1, 1, 1.f / currentScale));

			if (currentScale != player->standHeightScale)
			{
				standUpTimer += deltaTime;
				const float frac = FMath::Clamp(standUpTimer / player->crouchTransitionTime, 0.f, 1.f);
				const float newScale = FMath::Lerp(currentScale, player->standHeightScale, frac);

				SetRelativeScale3D(FVector(1, 1, newScale));
			}

			else
			{
				bNeedsToStand = false;
				standUpTimer = 0;
			}
		}
	}
}

bool UcringerTest::CheckIfGrounded()
{
	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	const float radius = bounds.X;
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

	bool bHitGround = player->GetWorld()->SweepMultiByChannel(
		GroundHits,
		player->GetActorLocation(),
		player->GetActorLocation(),
		FQuat::Identity,
		ECC_GameTraceChannel1,
		cap,
		param);

	if (bHitGround)
	{
		//If we hit anything iterate through each hit returned
		for (FHitResult hit : GroundHits)
		{
			//If the hit was level with the bottom of the collider
			if (hit.ImpactPoint.Z == GetComponentLocation().Z)
			{
				//Lets see if the normal of that surface is within our defined slope limit
				const float angle =
					FMath::RadiansToDegrees(
						FMath::Acos(
							FVector::DotProduct(FVector::UpVector, hit.ImpactNormal)));

				if (angle <= player->maxSlopeAngle)
				{
					if (hit.Component->GetCollisionProfileName() == FName(TEXT("Slide")))
						bOnSlide = true;
					else
						bOnSlide = false;

					return true;
				}
			}
		}
	}

	return false;
}

bool UcringerTest::CheckIfLedgeGrabEligible()
{
	//Start by sweeping our player region for a ledge

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FVector boundingBox = FVector(
		bounds.X * player->ledgeGrabRangeFactor,
		bounds.Y * player->ledgeGrabRangeFactor,
		bounds.Z);

	FCollisionShape box = FCollisionShape::MakeBox(boundingBox);

	bool bHitLedge = player->GetWorld()->SweepMultiByChannel(
		LedgeHits,
		GetComponentLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
		GetComponentLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
		FQuat::Identity,
		ECC_GameTraceChannel6,
		box,
		param);

	//If we hit something in this layer
	if (bHitLedge)
	{
		//Step through each hit
		for (FHitResult hit : LedgeHits)
		{
			//Lets make sure its exactly a ledge
			if (hit.Component->GetCollisionProfileName() == FName(TEXT("Ledge")))
			{
				//If they can, check if our player can reach the top of the collider 
				FVector ledgeBounds = hit.Component->Bounds.BoxExtent;
				ledgeBounds += hit.Actor.Get()->GetActorLocation();

				if (ledgeBounds.Z - player->GetActorLocation().Z <= player->ledgeGrabHeight)
				{
					//Calculate the normal of the way our player is looking (without Z)
					FVector camForward = player->camera->GetForwardVector();
					camForward.Z = 0;
					camForward.Normalize(0.01f);

					//Calculate the opposite of the ledge's normal
					FVector impactNormal = hit.ImpactNormal;
					impactNormal.Z = 0;
					impactNormal.Normalize(0.01f);
					impactNormal *= -1;

					//Calculate the Angle between the two vectors
					const float camDot = FVector::DotProduct(camForward, impactNormal);
					const float camAngle = FMath::RadiansToDegrees(FMath::Acos(camDot));

					//If the angle is small enough they're good to climb up
					if (camAngle <= player->ledgeLookAngle)
					{
						ULedgeGrabState::GetInstance()->SetLedge(hit);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool UcringerTest::CheckIfLedgeGrabEligible(FVector playerMoveVector)
{
	//Start by sweeping our player region for a ledge

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FVector boundingBox = FVector(
		bounds.X * player->ledgeGrabRangeFactor,
		bounds.Y * player->ledgeGrabRangeFactor,
		bounds.Z * .1f);

	FCollisionShape box = FCollisionShape::MakeBox(boundingBox);

	bool bHitLedge = player->GetWorld()->SweepMultiByChannel(
		LedgeHits,
		GetComponentLocation() + FVector(0, 0, bounds.Z) * 2.f,
		GetComponentLocation() + FVector(0, 0, bounds.Z) * 2.f,
		FQuat::Identity,
		ECC_GameTraceChannel6,
		box,
		param);

#if WITH_EDITOR

	DrawDebugBox(
		player->GetWorld(),
		GetComponentLocation() + FVector(0, 0, bounds.Z) * 2.f,
		boundingBox,
		FColor::Purple,
		false,
		.05f);
#endif

	//If we hit something in this layer
	if (bHitLedge)
	{
		//Step through each hit
		for (FHitResult hit : LedgeHits)
		{
			//Lets make sure its exactly a ledge
			if (hit.Component->GetCollisionProfileName() == FName(TEXT("Ledge")))
			{
				//If they can, check if our player can reach the top of the collider 
				FVector ledgeBounds = hit.Component->Bounds.BoxExtent;
				ledgeBounds += hit.Actor.Get()->GetActorLocation();

				if (ledgeBounds.Z - player->GetActorLocation().Z <= player->ledgeGrabHeight)
				{
					//Calculate the normal of the way our player is looking (without Z)
					FVector camForward = player->camera->GetForwardVector();
					camForward.Z = 0;
					camForward.Normalize(0.01f);

					//Calculate the opposite of the ledge's normal
					FVector impactNormal = hit.ImpactNormal;
					impactNormal.Z = 0;
					impactNormal.Normalize(0.01f);
					impactNormal *= -1;

					//Calculate the Angle between the two vectors
					const float camDot = FVector::DotProduct(camForward, impactNormal);
					const float camAngle = FMath::RadiansToDegrees(FMath::Acos(camDot));

					//If the angle is small enough lets see if the player is moving into the ledge
					if (camAngle <= player->ledgeLookAngle)
					{
						//Calculate the Angle between the two vectors
						const float moveDot = FVector::DotProduct(playerMoveVector, impactNormal);
						const float moveAngle = FMath::RadiansToDegrees(FMath::Acos(moveDot));

						//If the angle is small enough, the player is eligible to climb the ledge
						const bool movingInto = moveAngle <= player->ledgeLookAngle;

						if (movingInto)
						{
							ULedgeGrabState::GetInstance()->SetLedge(hit);
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool UcringerTest::CheckIfTryingToStand()
{

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FCollisionShape box = FCollisionShape::MakeBox(player->collider->bounds);

	bool bHitCeiling = player->GetWorld()->SweepSingleByChannel(
		player->collider->CrouchHitPoint,
		player->GetActorLocation() + FVector(0, 0, player->collider->bounds.Z) + FVector::UpVector,
		player->GetActorLocation() + FVector(0, 0, player->collider->bounds.Z) + FVector::UpVector,
		FQuat::Identity,
		ECC_Visibility,
		box,
		param);

#if WITH_EDITOR
	DrawDebugBox(
		player->GetWorld(),
		player->GetActorLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
		bounds,
		FColor::Purple,
		false,
		.05f);
#endif

	return bHitCeiling;
}