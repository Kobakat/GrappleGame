// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleGunComponent.h"
#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UGrappleGunComponent::UGrappleGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// Set default values
	FireRange = 3000.F;
	MinLength = 250.F;
	MaxLength = 3000.F;
	ShotSpeed = 3500.F;
}


// Called when the game starts
void UGrappleGunComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UGrappleGunComponent::SetCastingFromComponent(USceneComponent * CastingFrom)
{
	CastingFromComponent = CastingFrom;
}

bool UGrappleGunComponent::GetCanAttach()
{
	return CanAttach;
}

bool UGrappleGunComponent::GetIsAnimating()
{
	return IsRetracting || IsShooting;
}

float UGrappleGunComponent::GetLength()
{
	return Length;
}

FVector UGrappleGunComponent::GetAttachedLocation()
{
	// Transform the local grapple point into world space
	if (CurrentHookedActor != nullptr)
		return CurrentHookedActor->GetActorTransform().TransformPosition(LocalAttachedLocation);
	// Just in case something goes wrong
	else
		return FVector::ZeroVector;
}

FVector UGrappleGunComponent::GetGunEnd()
{
	return GunEnd->GetComponentLocation();
}

void UGrappleGunComponent::Attach()
{
	// Check to see if there is a grapple reactor
	// that we should keep track of
	CurrentHookedActor = LastHitActor;
	AGrappleReactor* reactor = Cast<AGrappleReactor>(LastHitActor);
	if (IsValid(reactor))
	{
		CurrentReactor = reactor;
		CurrentReactor->Hook(LastHitLocation);
	}
	else
		CurrentReactor = nullptr;
	// Get the local location of the grapple hit
	LocalAttachedLocation = CurrentHookedActor->GetActorTransform()
		.InverseTransformPosition(LastHitLocation);
	// Set the initial shooting state
	TArray<FVector> points;
	points.Add(CastingFromComponent->GetComponentLocation());
	points.Add(CastingFromComponent->GetComponentLocation());
	Polyline->SetAllPoints(points);
	Length = 0.F;
	IsShooting = true;
	IsRetracting = false;
	IsAttached = false;
	// Notify audio logic
	OnGrappleShot.Broadcast();
	OnGrappleStartedTraveling.Broadcast();
}

void UGrappleGunComponent::Detach()
{
	// Tell the grapple reactor that it is no longer hooked
	AGrappleReactor* reactor = Cast<AGrappleReactor>(LastHitActor);
	if (IsValid(reactor))
		CurrentReactor->Unhook();
	IsRetracting = true;
	IsAttached = false;
	// Notify audio logic
	OnGrappleStartedTraveling.Broadcast();
}

void UGrappleGunComponent::ApplyForce(FVector pullPoint, FVector pullTowards, float desiredDistance)
{
	// Relay the force if we are hooked to a reactor
	if (IsValid(CurrentReactor))
		CurrentReactor->ApplyPullForce(pullPoint, pullTowards, desiredDistance);
}

void UGrappleGunComponent::Reel(float value)
{
	// Apply the reel, but do not exceed the limits
	Length = FMath::Clamp(Length + value, MinLength, MaxLength);
}

void UGrappleGunComponent::SetIsRendered(bool rendered)
{
	isRendered = rendered;
	if (!isRendered)
		CanAttach = false;
}

// Called every frame
void UGrappleGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Only run logic if the component is rendered
	if (isRendered)
	{
		// Handle animation state
		if (IsShooting)
		{
			Polyline->SetLastPoint(GetGunEnd() + GetOwner()->GetVelocity() * GetWorld()->DeltaTimeSeconds);
			// How much further to attach?
			FVector current = Polyline->GetPoints()[0];
			FVector target = GetAttachedLocation();
			float deltaDistance = DeltaTime * ShotSpeed;
			if (deltaDistance > FVector::Distance(current, target))
			{
				// Finish the shoot animation
				// This will pass control to the grapple state
				Polyline->SetFirstPoint(target);
				IsShooting = false;
				IsAttached = true;
				// Notify audio logic
				OnGrappleHit.Broadcast();
				OnGrappleStoppedTraveling.Broadcast();
				// Set the initial length for the state to work with
				LastFrameLength = 
				Length = FVector::Distance(
					CastingFromComponent->GetComponentLocation(), target);
			}
			else
			{
				// Move towards the hit target
				Polyline->SetFirstPoint(
					current + (target - current).GetUnsafeNormal() * deltaDistance);
			}
		}
		else if (IsRetracting)
		{
			Polyline->SetLastPoint(GetGunEnd() + GetOwner()->GetVelocity() * GetWorld()->DeltaTimeSeconds);
			// Unwind the polyline
			float deltaDistance = DeltaTime * ShotSpeed;

			TArray<FVector> points = Polyline->GetPoints();
			while (deltaDistance > 0.F)
			{
				float segmentDistance = FVector::Distance(points[0], points[1]);
				if (deltaDistance > segmentDistance)
				{
					points.RemoveAt(0);
					deltaDistance -= segmentDistance;
					if (points.Num() == 1)
					{
						IsRetracting = false;
						Polyline->SetAllPoints(TArray<FVector>());
						// Notify audio logic
						OnGrappleStoppedTraveling.Broadcast();
						break;
					}
				}
				else
				{
					points[0] += (points[1] - points[0]).GetUnsafeNormal() * deltaDistance;
					deltaDistance = 0.F;
				}
			}
			if (IsRetracting)
				Polyline->SetAllPoints(points);
		}
		else
		{
			float deltaReel = Length - LastFrameLength;
			LastFrameLength = Length;
			if (isReeling)
			{
				// We have switched directions
				if (deltaReel > 0.F)
				{
					isReeling = false;
					isUnreeling = true;
					OnStoppedReelingIn.Broadcast();
					OnStartedReelingOut.Broadcast();
				}
				// We have stopped reeling
				else if (deltaReel == 0.F)
				{
					isReeling = false;
					OnStoppedReelingIn.Broadcast();
				}
			}
			else if (isUnreeling)
			{
				// We have switched directions
				if (deltaReel < 0.F)
				{
					isReeling = true;
					isUnreeling = false;
					OnStoppedReelingOut.Broadcast();
					OnStartedReelingIn.Broadcast();
				}
				// We have stopped reeling
				else if (deltaReel == 0.F)
				{
					isUnreeling = false;
					OnStoppedReelingOut.Broadcast();
				}
			}
			else
			{
				if (deltaReel > 0.F)
				{
					isUnreeling = true;
					OnStartedReelingOut.Broadcast();
				}
				else if (deltaReel < 0.F)
				{
					isReeling = true;
					OnStartedReelingIn.Broadcast();
				}
			}
		}
		// Update the grapple end
		if (Polyline->GetPoints().Num() > 0)
			GrappleHookEnd->SetWorldLocation(Polyline->GetPoints()[0]);
		else
			GrappleHookEnd->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
		// Only cast if the cast from component has been properly initialized
		if (CastingFromComponent != nullptr)
		{
			// Create a cast along the line of the grapple
			FVector Start = CastingFromComponent->GetComponentLocation();
			FVector End = Start + CastingFromComponent->GetForwardVector() * FireRange;
			// Ignore collision with other player colliders
			FCollisionQueryParams Params;
			Params.AddIgnoredActors(IgnoredActors);
			// Create space for the cast result
			FHitResult Result;

			// Is there a grapple surface within range
			if (GetWorld()->LineTraceSingleByChannel(
				Result, Start, End, ECC_GameTraceChannel3, Params))
			{
				// Check for a grapple blocker that may be closer
				FHitResult BlockerResult;
				if (GetWorld()->LineTraceSingleByChannel(
					BlockerResult, Start, End, ECC_GameTraceChannel4, Params)
					&&
					// Is the blocking object closer
					(BlockerResult.Location - Start).SizeSquared()
					< (Result.Location - Start).SizeSquared())
				{
					// There is a closer object blocking the grapple
					CanAttach = false;
				}
				else
				{
					// We can currently attach
					CanAttach = true;
					LastHitActor = Result.GetActor();
					LastHitLocation = Result.Location;
				}
			}
			else
				CanAttach = false;
		}
	}
}
