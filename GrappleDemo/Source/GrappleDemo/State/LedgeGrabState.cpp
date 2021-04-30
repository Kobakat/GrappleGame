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

void ULedgeGrabState::SetLedge(FHitResult newLedge, FHitResult newLedgeTop)
{
	ledge = newLedge;
	ledgeTop = newLedgeTop;
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
	player->collider->SetEnableGravity(false);
	player->collider->SetPhysicsLinearVelocity(FVector::ZeroVector);
	InitializeLedgeValues();
	player->OnLedgeClimb.Broadcast();
}

void ULedgeGrabState::StateTick(float deltaTime)
{
	DeterminePlayerAction(deltaTime);
}

void ULedgeGrabState::OnStateExit()
{
	player->collider->SetEnableGravity(true);	
}

void ULedgeGrabState::InitializeLedgeValues()
{
	this->startLoc = player->collider->GetComponentLocation();
	this->liftHeight = player->collider->LedgeTop.ImpactPoint.Z + player->standHeight;
	this->pushDir = player->camera->GetForwardVector();
	this->pushDir.Z = 0;
	this->pushDir.Normalize(0.001f);
	this->topPoint = ledgeTop.ImpactPoint;
	this->pushTimer = 0;
	this->bNeedsToPush = false;
}

void ULedgeGrabState::DeterminePlayerAction(float deltaTime) 
{	
	if (bNeedsToPush)
		PushPlayerForward(deltaTime);
	else
		LiftPlayerUp(deltaTime);
}

void ULedgeGrabState::LiftPlayerUp(float deltaTime)
{
	const float newHeight = player->collider->GetComponentLocation().Z + player->ledgeClimbSpeed * deltaTime;

	player->collider->SetRelativeLocation(FVector(
		startLoc.X,
		startLoc.Y,
		newHeight));

	if (newHeight >= liftHeight)
	{
		PushPlayerForward(deltaTime);
	}
}

void ULedgeGrabState::PushPlayerForward(float deltaTime)
{
	bNeedsToPush = true;

	FVector playerLoc = player->collider->GetComponentLocation();
	FVector targetXY = topPoint + (pushDir * player->bounds.X);
	FVector targetPoint;
	targetPoint.Z = topPoint.Z + player->standHeight;
	targetPoint.X = targetXY.X;
	targetPoint.Y = targetXY.Y;

	if (!playerLoc.Equals(targetPoint, 5.f))
	{
		pushTimer += deltaTime;
		const FVector playerXY = FVector(startLoc.X, startLoc.Y, targetPoint.Z);
		const float frac = FMath::Clamp(pushTimer / .1f, 0.f, 1.f);

		const FVector newLoc = FMath::Lerp(playerXY, targetPoint, frac);

		player->collider->SetRelativeLocation(newLoc + FVector::UpVector);
	}

	else 
	{
		//Determine what kind of push we should give to the player based on their input
		if (!player->moveVector.IsNearlyZero(0.01f) && player->tryingToSprint)
			pushType = LPT_Fast;
		else if (!player->moveVector.IsNearlyZero(0.01f))
			pushType = LPT_Slow;
		else
			pushType = LPT_Lazy;
	
		switch (pushType)
		{
		case LPT_Lazy:
			pushDir *= player->ledgePushSpeed;
			player->collider->SetPhysicsLinearVelocity(pushDir);
			player->SetState(UIdleState::GetInstance());
			break;
		case LPT_Slow:
			pushDir *= player->walkMaxSpeed;
			player->collider->SetPhysicsLinearVelocity(pushDir);
			player->SetState(UWalkState::GetInstance());
			break;
		case LPT_Fast:
			pushDir *= player->runMaxSpeed;
			player->collider->SetPhysicsLinearVelocity(pushDir);
			player->SetState(URunState::GetInstance());
			break;
		}
	}
}