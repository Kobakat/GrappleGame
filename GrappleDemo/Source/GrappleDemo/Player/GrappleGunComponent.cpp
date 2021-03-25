// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleGunComponent.h"
#include "Components/SceneComponent.h"
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
	// Set the initial length
	Length = FVector::Distance(
		CastingFromComponent->GetComponentLocation(),
		LastHitLocation);
	IsAttached = true;
}

void UGrappleGunComponent::Detach()
{
	// Tell the grapple reactor that it is no longer hooked
	AGrappleReactor* reactor = Cast<AGrappleReactor>(LastHitActor);
	if (IsValid(reactor))
		CurrentReactor->Unhook();
	// Reset the grapple hook back to the gun
	GrappleHookEnd->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	IsAttached = false;
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

// Called every frame
void UGrappleGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// Update the grapple end if we are attached
	if (IsAttached)
	{
		GrappleHookEnd->SetWorldLocation(GetAttachedLocation());
	}
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
