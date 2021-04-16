#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LevelPreviewNode.generated.h"

UENUM()
enum ELevelPreviewTransition
{
	Cut,
	Linear,
	SmoothStep,
	Ease_in,
	Ease_out
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class GRAPPLEDEMO_API ULevelPreviewNode : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULevelPreviewNode();

	FVector pos;
	FRotator rot;

	UPROPERTY(EditAnywhere, Category = "Level Preview")
		TEnumAsByte<ELevelPreviewTransition> transitionType;
	UPROPERTY(EditAnywhere, Category = "Level Preview")
		int index;
	UPROPERTY(EditAnywhere, Category = "Level Preview")
		float holdTime;
	UPROPERTY(EditAnywhere, Category = "Level Preview")
		float transitionTime;

protected:
	virtual void BeginPlay() override;
};
