#include "LedgeGrabState.h"
#include "../Player/PlayerPawn.h"

ULedgeGrabState::ULedgeGrabState() { }
ULedgeGrabState::~ULedgeGrabState() { }
ULedgeGrabState* ULedgeGrabState::instance;
ULedgeGrabState* ULedgeGrabState::GetInstance()
{
	if (instance == nullptr)
	{
		instance = NewObject<ULedgeGrabState>();
	}
	return instance;
}

#pragma region State Events

void ULedgeGrabState::Initialize(APlayerPawn* pawn)
{
	this->stateName = "Ledge Grab";
	UState::Initialize(pawn);
}

void ULedgeGrabState::OnStateEnter()
{
	player->stateName = this->stateName;
	player->collider->SetSimulatePhysics(false);
	this->climbTimer = 0;
	this->pushTimer = 0;
	this->startLoc = player->GetActorLocation();

	//Get the height the player is climbing to
	//TODO this will not work if the ledge moves up and down
	FVector ledgeBounds = player->LedgeHitPoint.Component->Bounds.BoxExtent;
	ledgeBounds += player->LedgeHitPoint.Actor.Get()->GetActorLocation();
	this->liftHeight = ledgeBounds.Z;

	//Get the direction to push the player
	this->pushDir = player->LedgeHitPoint.Normal * -1;
}

void ULedgeGrabState::StateTick(float deltaTime)
{
	DeterminePlayerAction(deltaTime);
}

void ULedgeGrabState::OnStateExit()
{
	player->collider->SetSimulatePhysics(true);
	player->collider->SetPhysMaterialOverride(player->stopMat);
	bClimbComplete = false;
	bPushComplete = false;
}

void ULedgeGrabState::DeterminePlayerAction(float deltaTime) 
{	
	if (!this->bClimbComplete)
	{
		LiftPlayerUp(deltaTime);
	}

	else if (!this->bPushComplete)
	{
		PushPlayerForward(deltaTime);
	}

	else 
	{		
		player->SetState(UIdleState::GetInstance());
	}

}
void ULedgeGrabState::LiftPlayerUp(float deltaTime)
{
	climbTimer += deltaTime;
	FVector playerLoc = player->GetActorLocation();

	const float frac = FMath::Clamp(climbTimer / player->ledgeVerticalTransitionTime, 0.f, 1.f);
	const float heightDifference = liftHeight - startLoc.Z;

	player->collider->SetRelativeLocation(FVector(playerLoc.X, playerLoc.Y, startLoc.Z + (heightDifference * frac)));

	if (frac >= 1)
		bClimbComplete = true;
}
void ULedgeGrabState::PushPlayerForward(float deltaTime)
{
	pushTimer += deltaTime;
	FVector playerLoc = player->GetActorLocation();

	const float frac = FMath::Clamp(pushTimer / player->ledgeHorizontalTransitionTime, 0.f, 1.f);
	
	player->collider->SetRelativeLocation(FVector(startLoc.X, startLoc.Y, playerLoc.Z) + (pushDir * frac * player->ledgePushDistance));

	if (frac >= 1)
		bPushComplete = true;
}

