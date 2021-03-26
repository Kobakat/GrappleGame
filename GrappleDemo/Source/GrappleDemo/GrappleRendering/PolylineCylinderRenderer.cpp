// Fill out your copyright notice in the Description page of Project Settings.


#include "PolylineCylinderRenderer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APolylineCylinderRenderer::APolylineCylinderRenderer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetTickGroup(ETickingGroup::TG_PostPhysics);
}

void APolylineCylinderRenderer::SetCylinder(int endIndex, FVector start, FVector end)
{
	// Position the cylinder in the center of the points.
	Cylinders[endIndex - 1]->SetWorldLocation((Points[endIndex - 1] + Points[endIndex]) * 0.5F);
	// Rotate look in the direction along the two points.
	Cylinders[endIndex - 1]->SetWorldRotation(
		UKismetMathLibrary::MakeRotFromXZ(
			Points[endIndex] - Points[endIndex - 1],
			FVector::UpVector));
	// Scale the cylinder such that it lines up with segment end spheres.
	float length = FVector::Distance(Points[endIndex - 1], Points[endIndex]);
	Cylinders[endIndex - 1]->SetRelativeScale3D(FVector(
		length, Diameter, Diameter));
	// Scale the UV map to match the scaling.
	Materials[endIndex - 1]->SetScalarParameterValue("UTiling", length / Diameter);
}

// Called when the game starts or when spawned
void APolylineCylinderRenderer::BeginPlay()
{
	Super::BeginPlay();
	// Initialize the static mesh components.
	// Setting MaxPointCount here ensures intialization.
	// Otherwise it would require another exposed field.
	int maxCount = MaxPointCount;
	MaxPointCount = 0;
	SetMaxPointCount(maxCount);
	// Set all points using an empty array.
	// This will hide all instantiated components.
	SetAllPoints(TArray<FVector>());
	SetRadius(Diameter * 0.5F);
}

void APolylineCylinderRenderer::SetAllPoints(const TArray<FVector>& points)
{
	Points = points;
	// If the caller sent too many points in, ignore the last ones.
	if (Points.Num() > MaxPointCount)
		Points.SetNum(MaxPointCount);
	// Hide or show the pooled objects.
	for (int i = 0; i < MaxPointCount; i++)
	{
		Spheres[i]->SetHiddenInGame(i >= points.Num());
		if (i < MaxPointCount - 1)
			Cylinders[i]->SetHiddenInGame(i >= points.Num() - 1);
	}
	// Move all of the spheres to reflect the points.
	for (int i = 0; i < Points.Num(); i++)
		Spheres[i]->SetWorldLocation(Points[i]);
	// Move and scale all of the cylinders to connect between points.
	for (int i = 1; i < Points.Num(); i++)
		SetCylinder(i, Points[i - 1], Points[i]);
}

void APolylineCylinderRenderer::SetFirstPoint(FVector point)
{
	// Only execute if there is a point.
	if (Points.Num() > 0)
	{
		// Move the start point.
		Points[0] = point;
		Spheres[0]->SetWorldLocation(point);
		// If applicable move the first segment.
		if (Points.Num() > 1)
			SetCylinder(1, Points[0], Points[1]);
	}
}

void APolylineCylinderRenderer::SetLastPoint(FVector point)
{
	// How will this change effect the polyline?
	if (Points.Num() == 1)
	{
		// Just move the start point.
		Points[0] = point;
		Spheres[0]->SetWorldLocation(point);
	}
	else if (Points.Num() > 1)
	{
		// Move a point and the linking segment.
		int iEnd = Points.Num() - 1;
		Points[iEnd] = point;
		Spheres[iEnd]->SetWorldLocation(point);
		SetCylinder(iEnd, Points[iEnd - 1], Points[iEnd]);
	}
}

bool APolylineCylinderRenderer::PushPoint(FVector point)
{
	// There is no more space. Max point count would
	// have to be increased.
	if (Points.Num() >= MaxPointCount)
		return false;
	else
	{
		// Add the new space and set the corresponding point.
		int i = Points.Num();
		Points.SetNum(i + 1);
		// Reveal the new point and segment.
		Spheres[i]->SetHiddenInGame(false);
		if (i > 0)
			Cylinders[i - 1]->SetHiddenInGame(false);
		// Ensure positioning on the last point.
		SetLastPoint(point);
		return true;
	}
}

bool APolylineCylinderRenderer::PopPoint()
{
	// There is nothing to pop.
	if (Points.Num() == 0)
		return false;
	else
	{
		// Remove the last point.
		int i = Points.Num();
		Points.SetNum(i - 1);
		// Hide the last point and segment.
		Spheres[i - 1]->SetHiddenInGame(true);
		if (i > 1)
			Cylinders[i - 2]->SetHiddenInGame(true);
		return true;
	}
}

void APolylineCylinderRenderer::Tick(float deltaTime)
{
	if (currentEnd != nullptr)
	{
		SetLastPoint(currentEnd->GetComponentLocation());
	}
}

void APolylineCylinderRenderer::SetRadius(float radius)
{
	// Set the underlying field.
	Diameter = radius * 2.F;
	// On each cylinder segment set the scale to reflect
	// the new radius.
	for (UStaticMeshComponent* cylinder : Cylinders)
	{
		cylinder->SetRelativeScale3D(FVector(
			cylinder->GetRelativeScale3D().X,
			Diameter,
			Diameter));
	}
	// Do the same for the spheres, but on all axes.
	for (UStaticMeshComponent* sphere : Spheres)
		sphere->SetWorldScale3D(FVector::OneVector * Diameter);
}

void APolylineCylinderRenderer::SetMaxPointCount(int count)
{
	// Prevent invalid input with floor of 2.
	int newMaxCount = FMath::Max(count, 2);
	// If the new max is less than the current, then
	// remove the mesh components at the end of the collection.
	for (int i = MaxPointCount - 2; i > newMaxCount - 2; i--)
		Cylinders[i]->DestroyComponent();
	for (int i = MaxPointCount - 1; i > newMaxCount - 1; i--)
		Spheres[i]->DestroyComponent();
	// Resize the arrays.
	Spheres.SetNum(newMaxCount);
	Cylinders.SetNum(newMaxCount - 1);
	Materials.SetNum(newMaxCount - 1);
	// Instantiate new mesh components if needed.
	for (int i = MaxPointCount; i < newMaxCount; i++)
	{
		UStaticMeshComponent* NewSphere = NewObject<UStaticMeshComponent>(this);
		NewSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
		NewSphere->SetStaticMesh(SphereMesh);
		NewSphere->SetMaterial(0, SphereMaterial);
		NewSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		NewSphere->RegisterComponent();
		Spheres[i] = NewSphere;
		if (i < newMaxCount - 1)
		{
			UMaterialInstanceDynamic* NewMaterial = UMaterialInstanceDynamic::Create(CylinderMaterialInstance, this);
			Materials[i] = NewMaterial;
			UStaticMeshComponent* NewCylinder = NewObject<UStaticMeshComponent>(this);
			NewCylinder->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
			NewCylinder->SetStaticMesh(CylinderMesh);
			NewCylinder->SetMaterial(0, NewMaterial);
			NewCylinder->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			NewCylinder->RegisterComponent();
			Cylinders[i] = NewCylinder;
		}
	}
	// Update underlying value.
	MaxPointCount = newMaxCount;
}
