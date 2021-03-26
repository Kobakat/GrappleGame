// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Array.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/UObjectGlobals.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "PolylineCylinderRenderer.generated.h"

UCLASS()
class GRAPPLEDEMO_API APolylineCylinderRenderer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APolylineCylinderRenderer();

private:
	void SetCylinder(int endIndex, FVector start, FVector end);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Holds the cylinders and spheres for drawing this cable.
	TArray<UStaticMeshComponent*> Cylinders;
	TArray<UStaticMeshComponent*> Spheres;
	TArray<UMaterialInstanceDynamic*> Materials;
	// The points that make up the polyline.
	TArray<FVector> Points;
	// The radius of the polyline.
	UPROPERTY(EditAnywhere, Category = "Polyline", meta = (ClampMin = "2.0"))
	float Diameter = 50.F;
	// The maximum number of points on the polyline.
	UPROPERTY(EditAnywhere, Category = "Polyline", meta = (ClampMin = "2"))
	int MaxPointCount = 2;

	// The cylinder mesh used along the polyline segments.
	UPROPERTY(EditAnywhere, Category = "Polyline")
	UStaticMesh* CylinderMesh;
	// The cylinder mesh used along the polyline segments.
	UPROPERTY(EditAnywhere, Category = "Polyline")
	UStaticMesh* SphereMesh;
	// Used to texture the cylinders dynamically.
	UPROPERTY(EditAnywhere, Category = "Polyline")
	UMaterialInterface* CylinderMaterialInstance;
	// Used to texture the cylinders dynamically.
	UPROPERTY(EditAnywhere, Category = "Polyline")
	UMaterial* SphereMaterial;

public:	
	// Sets all points of the polyline, recalculating everything.
	void SetAllPoints(const TArray<FVector>& points);
	// Sets the first point of the polyline.
	void SetFirstPoint(FVector point);
	// Sets the last point of the polyline.
	void SetLastPoint(FVector point);
	// Adds a new point onto the end of the polyline.
	// Returns false if the polyline is at its limit and cannot add the point.
	bool PushPoint(FVector point);
	// Removes the last point from the polyline.
	// Returns false if the polyline contains zero points.
	bool PopPoint();

	USceneComponent* currentEnd;
	virtual void Tick(float deltaTime) override;

	// Sets the radius of the polyline.
	void SetRadius(float radius);
	// Sets the maximum point count for this polyline.
	void SetMaxPointCount(int count);
};
