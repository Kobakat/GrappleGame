#include "GrappleState.h"
#include "DrawDebugHelpers.h"
#include "../../Player/PlayerPawn.h"

#pragma region State Initialization
void UGrappleState::Initialize(APlayerPawn* pawn)
{
	UState::Initialize(pawn);
	// Retrieve the grapple components.
	grappleComponent = pawn->grappleComponent;
	grapplePolyline = pawn->GrapplePolyline;
	// Set the number of raycasts used for the
	// wrap solving at each wrap pivot.
	WrapCheckIterations = 10;
}
#pragma endregion
#pragma region State Enter and Exit
void UGrappleState::OnStateEnter()
{
	UMovementState::OnStateEnter();
	// Reset parameters for wrapping.
	WrapPivots.Empty();
	WrapActors.Empty();
	grapplePolyline->SetAllPoints(TArray<FVector>());
	grapplePolyline->PushPoint(grappleComponent->GetAttachedLocation());
	grapplePolyline->PushPoint(grappleComponent->GetGunEnd());
}
void UGrappleState::OnStateExit()
{
	UMovementState::OnStateExit();
	// Detach the grapple hook
	grappleComponent->Detach();
	// Clear out the polyline renderer
	grapplePolyline->SetAllPoints(TArray<FVector>());
}
#pragma endregion
#pragma region Solve Restraint
bool UGrappleState::SolveRestraint()
{
	// Solve the wrapping first to determine the current pivot.
	SolveWrap();
	// Transform the pivots from object space into global space.
	FVector attachedLocation = grappleComponent->GetAttachedLocation();
	TArray<FVector> GlobalPivots;
	GlobalPivots.SetNum(WrapPivots.Num());
	for (int i = 0; i < WrapPivots.Num(); i++)
		GlobalPivots[i] = WrapActors[i]->GetTransform().TransformPosition(WrapPivots[i]);
	// Get the last pivot that we will solve relative to.
	FVector pivot = (GlobalPivots.Num() > 0)
		? GlobalPivots.Last() : attachedLocation;

	// Using the wrap data, calculate how much rope length is left.
	float lengthLeft = grappleComponent->GetLength();
	// Trace along the rope wrap subtracting distance.
	if (WrapPivots.Num() > 0)
		lengthLeft -= FVector::Distance(attachedLocation, GlobalPivots[0]);
	for (int i = 1; i < WrapPivots.Num(); i++)
		lengthLeft -= FVector::Distance(GlobalPivots[i], GlobalPivots[i - 1]);

	// Get the difference in length from the rope
	// radius to the grapple gun.
	FVector ropeVector = pivot - player->camera->GetComponentLocation();
	float lengthDifference = ropeVector.Size() - lengthLeft;

	// Are we inside the grapple radius?
	if (lengthDifference < 0.F)
	{
		// Automatically reel in the grapple to the closer distance.
		grappleComponent->Reel(lengthDifference);
	}
	// Are we outside the grapple radius?
	else if (lengthDifference > 0.F)
	{
		// Apply the pulling force to the reactor along the first rope segment.
		FVector pivotAfterReactor =
			(GlobalPivots.Num() > 0) ? GlobalPivots[0] : player->camera->GetComponentLocation();
		float distanceBeforeReaction = FVector::Dist(attachedLocation, pivotAfterReactor);
		// Apply the pulling force to a grapple reactor.
		grappleComponent->ApplyForce(attachedLocation, pivotAfterReactor, distanceBeforeReaction - lengthDifference);
		attachedLocation = grappleComponent->GetAttachedLocation();
		// Peek how this has changed the distance.
		lengthDifference -= distanceBeforeReaction - FVector::Dist(attachedLocation, pivotAfterReactor);

		// Was the reactor unable to take up the pull force?
		// If so then the player needs to be pulled in.
		if (lengthDifference > 0.F)
		{
			// Get the direction vector and magnitude vector.
			FVector pullDirection = ropeVector.GetSafeNormal();
			FVector tensileDelta = pullDirection * lengthDifference;

			// Pull the player in.
			FVector beforePull = player->GetActorLocation();
			player->AddActorWorldOffset(tensileDelta, true);
			if ((player->GetActorLocation() - beforePull).SizeSquared() - lengthDifference * lengthDifference > 1.F)
			{
				// Not able to pull the player in!
				return false;
			}
			else
			{
				// Get the current player velocity, updating it to reflect
				// the pulling force of the grapple rope.
				FVector velocity = player->collider->GetPhysicsLinearVelocity()
					+ tensileDelta / grappleComponent->GetWorld()->DeltaTimeSeconds;
				FVector tangentPoint = player->camera->GetComponentLocation();

				// Is the velocity pointing outwards?
				if (FVector::Distance(pivot, tangentPoint) <= FVector::Distance(pivot, tangentPoint + velocity))
				{
					// Project the player velocity such that it is tangent to the
					// sphere of the rope radius.
					velocity = FVector::PointPlaneProject(
						tangentPoint + velocity,
						tangentPoint,
						-pullDirection) - tangentPoint;
				}
				// Set the player velocity.
				player->collider->SetPhysicsLinearVelocity(velocity);
			}
		}
	}

	// Update the grapple rope.
	TArray<FVector> points;
	points.SetNum(GlobalPivots.Num() + 2);
	points[0] = attachedLocation;
	for (int i = 0; i < GlobalPivots.Num(); i++)
		points[i + 1] = GlobalPivots[i];
	points[points.Num() - 1] = grappleComponent->GetGunEnd()
		+ player->collider->GetPhysicsLinearVelocity() * player->GetWorld()->DeltaTimeSeconds;
	grapplePolyline->SetAllPoints(points);
	return true;
}
#pragma endregion
#pragma region Solve Wrap
void UGrappleState::SolveWrap()
{
	// Grab key locations.
	UWorld* world = grappleComponent->GetWorld();
	FVector hookLocation = grappleComponent->GetAttachedLocation();
	FVector gunTipLocation = grappleComponent->GetGunEnd();
	FVector camLocation = player->camera->GetComponentLocation();
	// Create fields for raycasts.
	FHitResult GrappleHitPoint;
	FCollisionQueryParams CollisionParams;

	// Handle any unwrapping.
	if (WrapPivots.Num() > 0)
	{
		// Step backwards, checking if each pivot can be unwrapped.
		for (int i = WrapPivots.Num() - 1; i >= 0; i--)
		{
			// Look one pivot further, or to the hook location,
			// to determine what the unwrapped segment would look like.
			FVector nextPivot = ((i == 0) ? hookLocation : WrapActors[i - 1]->GetTransform().TransformPosition(WrapPivots[i - 1]));
			// Is there a clear line around this wrap point?
			// If so unwind the pivot, otherwise stop checking pivots.
			if (!world->LineTraceSingleByChannel(GrappleHitPoint,
				nextPivot, camLocation, ECC_GameTraceChannel4, CollisionParams))
			{
				// Project the pivot onto the new prospective unwrapped segment.
				FVector unwrapCheck = (WrapActors[i]->GetTransform().TransformPosition(WrapPivots[i]) - camLocation).
					ProjectOnTo(nextPivot - camLocation) + camLocation;
				// See if the unwrap can be done without an intersection.
				// This is what establishes the normal context of the pivots.
				if (!world->LineTraceSingleByChannel(GrappleHitPoint,
					WrapActors[i]->GetTransform().TransformPosition(WrapPivots[i]), unwrapCheck, ECC_GameTraceChannel4, CollisionParams))
				{
					WrapPivots.Pop();
					WrapActors.Pop();
					grapplePolyline->PopPoint();
				}
				else
					break;
			}
			else
				break;
		}
	}
	// Handle any wrapping.
	// Wrapping is always relative to the final wrap pivot.
	FVector Start = camLocation;
	FVector End = ((WrapPivots.Num() == 0) ? hookLocation : WrapActors.Last()->GetTransform().TransformPosition(WrapPivots.Last()));
	int loops = 0;
	// Keep checking the cable line until there is a free path to the grapple end.
	// Wrap as many times as is needed.
	while (world->LineTraceSingleByChannel(GrappleHitPoint, End, Start, ECC_GameTraceChannel4, CollisionParams))
	{
		// This is a safety check to avoid a while loop
		// crash. This might trigger if the grapple gets stuck inside
		// a volume and can't get out.
		loops++;
		if (loops > 50) break;
		// Initialize variables for bisection.
		float interpolant = 0.5F;
		float bisection = 0.5F;
		FHitResult lastHitResult;
		// Run bisection to iteratively approach the actual edge
		// of the geometry to wrap around.
		for (int i = 0; i < WrapCheckIterations; i++)
		{
			bisection *= 0.5F;
			// Move towards the last frame's position that was
			// known to be non-obscured.
			Start = FMath::Lerp(camLocation, LastFramePlayerLocation, interpolant);
			// If we are still hitting then continue further towards last frame.
			if (world->LineTraceSingleByChannel(GrappleHitPoint,
				End, Start, ECC_GameTraceChannel4, CollisionParams))
			{
				interpolant += bisection;
				// Record this result.
				lastHitResult = GrappleHitPoint;
			}
			else
			{
				// If we cleared then step back
				// towards the edge.
				interpolant -= bisection;
			}
		}
		// Use the last found hit, should be pretty close to the edge,
		// and add some clearence (otherwise future casts will get trapped in the mesh).
		FVector normal = (LastFramePlayerLocation - camLocation).GetSafeNormal();
		FVector wrapPoint = lastHitResult.Location + normal * 5.F;
		// Push the pivot data and repeat if neccasary.
		WrapPivots.Add(lastHitResult.GetActor()->GetTransform().InverseTransformPosition(wrapPoint));
		WrapActors.Add(lastHitResult.GetActor());
		End = wrapPoint;
	}
	// Update last stored location.
	LastFramePlayerLocation = camLocation;
}
#pragma endregion
