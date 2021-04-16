#include "LevelPreviewNode.h"

ULevelPreviewNode::ULevelPreviewNode() { PrimaryComponentTick.bCanEverTick = false; }

void ULevelPreviewNode::BeginPlay()
{
	Super::BeginPlay();

	pos = GetComponentLocation();
	rot = GetComponentRotation();
}