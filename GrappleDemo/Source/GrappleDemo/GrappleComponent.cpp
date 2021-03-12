#include "GrappleComponent.h"

UGrappleComponent::UGrappleComponent() { PrimaryComponentTick.bCanEverTick = true; }
UGrappleComponent::~UGrappleComponent() { }

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	EndLocation = GetAttachedLocation() - GetComponentLocation();
	UCableComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FVector UGrappleComponent::GetAttachedLocation()
{
	if (attachedActor != nullptr)
		/*return attachedLocation + attachedActor->GetActorLocation();*/
		return attachedActor->GetActorTransform().TransformPosition(attachedLocation);
	else
		return FVector::ZeroVector;
}

float UGrappleComponent::GetCableLength()
{
	return currentCableLength;
}

void UGrappleComponent::ApplyForce(FVector force)
{
	if (IsValid(grappleReactor))
	{
		grappleReactor->ApplyPullForce(force);
	}
}

void UGrappleComponent::Reel(float value)
{

	if (currentCableLength + value * grappleReelSpeed >= 0.F && currentCableLength + value < grappleMaxDistance)
	{
		currentCableLength += value * grappleReelSpeed;
		CableLength = currentCableLength * grappleVisualMultiplier;
	}
	
}

void UGrappleComponent::Attach(FVector vector, AActor* actor)
{
	//attachedLocation = vector - actor->GetActorLocation();
	attachedLocation = actor->GetActorTransform().InverseTransformPosition(vector);
	attachedActor = actor;

	if (IsValid(grappleReactor))
	{
		grappleReactor->Hook(vector);
	}

	bAttachStart = true;

	// gets the distance between the attach point and the start of the cable
	currentCableLength = FMath::Min(FVector::Distance(vector, GetComponentLocation()), grappleMaxDistance);
	CableLength = currentCableLength * grappleVisualMultiplier;
}

void UGrappleComponent::Detach()
{
	//bAttachStart = false;
}