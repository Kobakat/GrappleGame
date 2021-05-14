#include "LevelPreviewPawn.h"
#include "LevelPreviewNode.h"
#include "GameFramework/PlayerController.h"
ALevelPreviewPawn::ALevelPreviewPawn()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevelPreviewPawn::BeginPlay()
{
	Super::BeginPlay();	
}

void ALevelPreviewPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PerformStateAction(DeltaTime);
}

void ALevelPreviewPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ALevelPreviewPawn::EndPreview);
	InputComponent->BindAction("ShootRelease", IE_Pressed, this, &ALevelPreviewPawn::EndPreview);
	InputComponent->BindAction("InstantReel", IE_Pressed, this, &ALevelPreviewPawn::EndPreview);
	InputComponent->BindAction("Pause", IE_Pressed, this, &ALevelPreviewPawn::EndPreview);
}

void ALevelPreviewPawn::StartPreview()
{
	GetNodeInformation();

	OnPreviewStarted();
	state = Holding;
	for (ULevelPreviewNode* node : nodes)
	{
		node->Deactivate(); // turn off every child camera so only the parent camera renders
	}
}

void ALevelPreviewPawn::EndPreview()
{
	if (state != Stopping)
	{
		OnPreviewEnd();
		state = Stopping;
	}
}

void ALevelPreviewPawn::RepossessPlayer()
{
	AController* controller = GetController();
	controller->UnPossess();
	controller->Possess(playerPawn);
}

void ALevelPreviewPawn::GetNodeInformation()
{
	TArray<UActorComponent*> uncasted = GetComponentsByClass(ULevelPreviewNode::StaticClass());
	TArray<ULevelPreviewNode*> casted;

	for (UActorComponent* a : uncasted)
	{
		ULevelPreviewNode* node = Cast<ULevelPreviewNode>(a);
		casted.Add(node);
	}

	for (int i = 0; i < casted.Num(); i++)
	{
		ULevelPreviewNode* nodeAtIndex;

		for (ULevelPreviewNode* n : casted)
		{
			if (n->index == i + 1)
			{
				nodeAtIndex = n;
			}
		}

		nodes.Add(nodeAtIndex);
	}

	currentNode = nodes[0];
	SetActorLocation(currentNode->pos);
	SetActorRotation(currentNode->rot);
}

void ALevelPreviewPawn::PerformStateAction(float deltaTime)
{
	switch (state)
	{
	case Starting:
		break;
	case Holding:
		HoldAction(deltaTime);
		break;
	case Moving:
		MoveAction(deltaTime);
		break;
	case Stopping:
		break;
	}
}

void ALevelPreviewPawn::HoldAction(float deltaTime)
{
	timer += deltaTime;

	if (timer >= currentNode->holdTime)
	{
		if (currentNode->transitionTime <= 0.01f)
			MoveToNextNode();
		else
		{
			timer = 0;
			state = Moving;
		}
	}
}

void ALevelPreviewPawn::MoveAction(float deltaTime)
{
	timer += deltaTime;
	const float timerFrac = (timer / currentNode->transitionTime);
	float frac = FMath::Clamp(timerFrac, 0.f, 1.f);

	switch (currentNode->transitionType)
	{
	case Cut:
		frac = 1;
		break;
	case SmoothStep:
		frac = frac * frac * (3.f - 2.f * frac);
		break;
	case Ease_in:
		frac = frac * frac * frac * frac;
		break;	
	case Ease_out:
		frac = 1 - FMath::Pow(1 - frac, 4);
		break;
	}

	const FVector newLocation = FMath::Lerp(currentNode->pos, nodes[currentNode->index]->pos, frac);
	const FRotator newRot = FMath::Lerp(currentNode->rot, nodes[currentNode->index]->rot, frac);

	SetActorLocation(newLocation);
	SetActorRotation(newRot);

	if (frac >= 1.f)
	{
		//If this is the last node exit from the preview 
		if (currentNode->index + 1 > nodes.Num() - 1)
			EndPreview();

		//Otherwise move onto the next node
		else
		{
			MoveToNextNode();
		}
	}
}

void ALevelPreviewPawn::MoveToNextNode()
{
	int newNodeIndex = currentNode->index;

	if (newNodeIndex >= nodes.Num() - 1)
		EndPreview();
	else
	{
		timer = 0;
		state = Holding;
		currentNode = nodes[newNodeIndex];		
	}
}
