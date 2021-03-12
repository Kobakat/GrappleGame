// Fill out your copyright notice in the Description page of Project Settings.


#include "TorquePullableGrappleReactor.h"

ATorquePullableGrappleReactor::ATorquePullableGrappleReactor()
{

}

void ATorquePullableGrappleReactor::BeginPlay()
{

}

void ATorquePullableGrappleReactor::ApplyPullForce(const FVector force)
{
	// Get the force in local space.
	FVector localForce = GetActorTransform().InverseTransformVectorNoScale(force);
	FVector localAttach = localHookPosition;
	FVector localAxis;
	// Ignore the vector components aligned with the revolution axis.
	switch (Axis)
	{
	case EAxis::X:
		localForce.X = 0.F;
		localAttach.X = 0.F;
		localAxis = FVector::ForwardVector;
	case EAxis::Y:
		localForce.Y = 0.F;
		localAttach.Y = 0.F;
		localAxis = FVector::RightVector;
	case EAxis::Z:
		localForce.Z = 0.F;
		localAttach.Z = 0.F;
		localAxis = FVector::UpVector;
	}

	float angle = acosf(FVector::DotProduct(localAttach.GetSafeNormal(), (localAttach + localForce).GetSafeNormal()));

	float dist = FVector::PointPlaneDist(localAttach + localForce, localAttach, FVector::CrossProduct(localAxis, localAttach));

	if (dist < 0.F)
		angle *= -1.F;

	AddActorLocalRotation(FQuat(localAxis, angle));
}
