#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/InputComponent.h"
#include "LevelPreviewPawn.generated.h"

class ULevelPreviewNode;

UENUM()
enum EPreviewPawnState
{
	Starting,
	Holding,
	Moving,
	Stopping
};

UCLASS()
class GRAPPLEDEMO_API ALevelPreviewPawn : public APawn
{
	GENERATED_BODY()

public:

	ALevelPreviewPawn();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	APawn* playerPawn;
	void StartPreview();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<EPreviewPawnState> state;
	
	ULevelPreviewNode* currentNode;
	float timer;

	void EndPreview();
	void GetNodeInformation();
	void MoveToNextNode();
	void PerformStateAction(float deltaTime);
	void HoldAction(float deltaTime);
	void MoveAction(float deltaTime);

	TArray<ULevelPreviewNode*> nodes;
};