#include "PlayerCapsule.h"
#include "PlayerPawn.h"

UPlayerCapsule::UPlayerCapsule() { PrimaryComponentTick.bCanEverTick = false; }
void UPlayerCapsule::TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) { }

void UPlayerCapsule::BeginPlay()
{
	Super::BeginPlay();

	player = Cast<APlayerPawn>(GetAttachmentRootActor());
	bounds = CalculateBounds();
	halfHeight = GetScaledCapsuleHalfHeight();
}

FVector UPlayerCapsule::CalculateBounds()
{
	const float radius = GetScaledCapsuleRadius();
	const float height = GetScaledCapsuleHalfHeight() * 2.f;

	return FVector(
		radius,
		radius,
		height
	);
}

void UPlayerCapsule::HandleStandUp(float deltaTime)
{
	//TODO 
	//Discard hits against surfaces with an orthogonal to Z-up normal
	//This will let player stand up when directly against a wall

	if (bNeedsToStand)
	{
		const bool bHitCeiling = CheckIfTryingToStand();

		if (bHitCeiling)
		{
			bNeedsToStand = false;
			player->SetState(UCrouchState::GetInstance());
		}

		else
		{
			if (halfHeight != player->standHeight)
			{
				//Subtract time
				standUpTimer -= deltaTime;

				//Calculate a smooth step interpolant
				float frac = FMath::Clamp(standUpTimer / player->crouchTransitionTime, 0.f, 1.f);
				frac = frac * frac * (3.f - 2.f * frac);
				
				//Interoplate the collider/camera height
				const float newHeight = FMath::Lerp(player->standHeight, player->crouchHeight, frac);
				player->camera->baseHeight = FMath::Lerp(60, 30, frac); //HACK delete hard coded cam values
				
				//Set new values
				SetCapsuleHalfHeight(newHeight);
				player->camera->SetRelativeLocation(FVector::UpVector * player->camera->baseHeight);
					
				const FVector capsuleLoc = player->collider->GetComponentLocation();
				const FVector base = FVector(
					capsuleLoc.X,
					capsuleLoc.Y,
					capsuleLoc.Z - halfHeight);

				player->collider->halfHeight = player->collider->GetScaledCapsuleHalfHeight();

				const FVector newLoc = base + (FVector::UpVector * player->collider->halfHeight);
				player->collider->SetRelativeLocation(newLoc);
			}

			else
			{
				bNeedsToStand = false;
			}
		}
	}
}

bool UPlayerCapsule::CheckIfGrounded()
{
	const float radius = bounds.X * .99f;
	const FVector capsuleLoc = GetComponentLocation();
	const FVector sweepStart = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - halfHeight + bounds.X) + FVector::UpVector;
	const FVector sweepEnd = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - halfHeight) + FVector::DownVector;
	const FCollisionShape sphere = FCollisionShape::MakeSphere(radius);

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	
	const bool bHitSphere = player->GetWorld()->SweepMultiByChannel(
		GroundHits,
		sweepStart,
		sweepEnd,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		sphere,
		param);

	if (bHitSphere)
	{
		//If we hit anything iterate through each hit returned
		for (FHitResult hit : GroundHits)
		{
			//If the hit was level with the bottom of the collider
			if (hit.ImpactPoint.Z >= capsuleLoc.Z - halfHeight - .01f &&
				hit.ImpactPoint.Z <= capsuleLoc.Z - halfHeight + (bounds.X * 0.2f))
			{
				//Lets see if the normal of that surface is within our defined slope limit
				const float angle =
					FMath::RadiansToDegrees(
						FMath::Acos(
							FVector::DotProduct(FVector::UpVector, hit.Normal)));

				if (angle <= player->maxSlopeAngle)
				{
					if (hit.Component->GetCollisionProfileName() == FName(TEXT("Slide")) && GroundHits.Num() == 1)
					{
						bOnSlide = true;
						USlideState::GetInstance()->SetSlide(hit);
						return true;
					}

					else
					{
						bOnSlide = false;
						return true;
					}
				}
			}
		}
	}

	bOnSlide = false;
	return false;
}

bool UPlayerCapsule::CheckIfLedgeGrabEligible()
{
	//Start by sweeping our player region for a ledge

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FCollisionShape capsule = FCollisionShape::MakeCapsule(bounds.X, halfHeight);
	FVector camForward = player->camera->GetForwardVector();
	camForward.Z = 0;
	camForward.Normalize(0.01f);

	FVector init = GetComponentLocation() + FVector::UpVector;
	const bool bHitLedge = player->GetWorld()->SweepMultiByChannel(
		LedgeHits,
		init,
		init + (camForward * bounds.X * player->ledgeGrabRangeFactor),
		FQuat::Identity,
		ECC_GameTraceChannel6,
		capsule,
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
				//If it is, check if the player is looking at it

				//Calculate the opposite of the ledge's normal
				FVector impactNormal = hit.ImpactNormal;
				impactNormal.Z = 0;
				impactNormal.Normalize(0.01f);
				impactNormal *= -1;

				//Calculate the Angle between the two vectors
				const float camDot = FVector::DotProduct(camForward, impactNormal);
				const float camAngle = FMath::RadiansToDegrees(FMath::Acos(camDot));

				//If the angle is small enough, make sure they can reach the top of the ledge
				if (camAngle <= player->ledgeLookAngle)
				{
					FVector maxLedgeHeight = FVector::UpVector * player->ledgeMaxGrabHeight;

					FVector rayEnd = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, GetComponentLocation().Z) + impactNormal;
					FVector rayStart = rayEnd + maxLedgeHeight;

					const bool blockHit = player->GetWorld()->LineTraceSingleByChannel(
						LedgeTop,
						GetComponentLocation(),
						rayStart,
						ECC_Visibility,
						param
					);
					const bool ledgeHit = player->GetWorld()->LineTraceSingleByChannel(
						LedgeTop,
						rayStart,
						rayEnd,
						ECC_GameTraceChannel6,
						param
					);

					//If they can we just need to make sure that the ray doesn't start penetrating and exits the surface anywhere
					if (!blockHit
						&& ledgeHit
						&& ((LedgeTop.ImpactPoint.Z - GetComponentLocation().Z) >= player->ledgeMinGrabHeight))
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

bool UPlayerCapsule::CheckIfLedgeGrabEligible(FVector playerMoveVector)
{
	//Start by sweeping our player region for a ledge

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	FVector boundingBox = FVector(
		bounds.X * player->ledgeGrabRangeFactor,
		bounds.Y * player->ledgeGrabRangeFactor,
		bounds.Z * 0.05f);

	FCollisionShape box = FCollisionShape::MakeBox(boundingBox);

	bool bHitLedge = player->GetWorld()->SweepMultiByChannel(
		LedgeHits,
		GetComponentLocation() + (FVector::UpVector * halfHeight),
		GetComponentLocation() + (FVector::UpVector * halfHeight),
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
				//If it is, check if the player is looking at it

				//Calculate the opposite of the ledge's normal
				FVector impactNormal = hit.ImpactNormal;
				impactNormal.Z = 0;
				impactNormal.Normalize(0.01f);
				impactNormal *= -1;

				FVector camForward = player->camera->GetForwardVector();
				camForward.Z = 0;
				camForward.Normalize(0.01f);

				//Calculate the Angle between the two vectors
				const float camDot = FVector::DotProduct(camForward, impactNormal);
				const float camAngle = FMath::RadiansToDegrees(FMath::Acos(camDot));


				//If the angle is small enough, make sure they can reach the top of the ledge
				if (camAngle <= player->ledgeLookAngle)
				{
					//Calculate the player's move vector and the nor
					const float moveDot = FVector::DotProduct(playerMoveVector, impactNormal);
					const float moveAngle = FMath::RadiansToDegrees(FMath::Acos(moveDot));

					const bool movingInto = moveAngle <= player->ledgeLookAngle;

					if (movingInto)
					{
						FVector maxLedgeHeight = FVector::UpVector * player->ledgeMaxGrabHeight;

						FVector rayEnd = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, GetComponentLocation().Z) + impactNormal;
						FVector rayStart = rayEnd + maxLedgeHeight;

						const bool blockHit = player->GetWorld()->LineTraceSingleByChannel(
							LedgeTop,
							GetComponentLocation(),
							rayStart,
							ECC_Visibility,
							param
						);
						const bool ledgeHit = player->GetWorld()->LineTraceSingleByChannel(
							LedgeTop,
							rayStart,
							rayEnd,
							ECC_GameTraceChannel6,
							param
						);

						//If they can we just need to make sure that the ray doesn't start penetrating and exits the surface anywhere
						if (!blockHit
							&& ledgeHit
							&& ((LedgeTop.ImpactPoint.Z - GetComponentLocation().Z) >= player->ledgeMinGrabHeight))
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

bool UPlayerCapsule::CheckIfTryingToStand()
{
	const FVector capsuleLoc = GetComponentLocation();
	const FVector sweepStart = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - halfHeight + bounds.X) + FVector::UpVector;
	const FVector sweepEnd = FVector(capsuleLoc.X, capsuleLoc.Y, capsuleLoc.Z - halfHeight - bounds.X) + (FVector::UpVector * bounds.Z) + FVector::UpVector;
	const FCollisionShape sphere = FCollisionShape::MakeSphere(bounds.X);

	FCollisionQueryParams param;
	param.AddIgnoredActor(player);

	const bool bHitCeiling = player->GetWorld()->SweepMultiByChannel(
		CrouchHits,
		sweepStart,
		sweepEnd,
		FQuat::Identity,
		ECC_GameTraceChannel1,
		sphere,
		param);

	return bHitCeiling;
}

bool UPlayerCapsule::CheckIfStepUp(float& outHeight)
{
	/*if (player->bGrounded && !GetPhysicsLinearVelocity().IsNearlyZero(1.f))
	{
		for (FHitResult hit : BoxHits)
		{
			FHitResult step;
			FCollisionQueryParams param;
			param.AddIgnoredActor(player);

			FVector rayStart = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, GetComponentLocation().Z + player->stepHeight) + (hit.ImpactNormal * -1);
			FVector rayEnd = FVector(hit.ImpactPoint.X, hit.ImpactPoint.Y, GetComponentLocation().Z) + (hit.ImpactNormal * -1);

			const bool bHitStep = player->GetWorld()->LineTraceSingleByChannel(
				step,
				rayStart,
				rayEnd,
				ECC_GameTraceChannel1,
				param
			);

			if (bHitStep)
			{
				const float hitHeight = step.Location.Z;
				const float top = GetComponentLocation().Z + player->stepHeight;
				const float bot = GetComponentLocation().Z + .001f;

				if (hitHeight <= top
					&& hitHeight >= bot)
				{
					outHeight = hitHeight;
					return true;
				}
			}
		}
	}

	return false;*/

	return false;
}

float UPlayerCapsule::GetCrouchTime(float targetScale)
{
	const float maxDistance = player->standHeight - player->crouchHeight;
	const float distance = FMath::Abs(halfHeight - targetScale);
	const float frac = distance / maxDistance;

	return frac * player->crouchTransitionTime;
}
