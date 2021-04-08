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

FHitResult ULedgeGrabState::ledge;
void ULedgeGrabState::SetLedge(FHitResult newLedge)
{
	ledge = newLedge;
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
	this->climbDistance = liftHeight - startLoc.Z;
	this->pushDir = -ledge.ImpactNormal;

	if (player->bGrounded)
	{
		if (player->collider->LedgeTop.ImpactPoint.Z > player->camera->GetComponentLocation().Z)
		{
			climbType = LCT_AboveHead;
			climbDuration = player->ledgeAboveHeadTime;			
		}
			
		else if (player->collider->LedgeTop.ImpactPoint.Z < startLoc.Z)
		{
			climbType = LCT_UnderWaist;
			climbDuration = player->ledgeUnderWaistTime;
		}
						
		else
		{
			climbType = LCT_AboveWaist;
			climbDuration = player->ledgeAboveWaistTime;
		}
			
	}

	else
	{
		climbType = LCT_Airborne;
		climbDuration = player->ledgeAirborneTime;
	}

	climbTimer = climbDuration;
}

void ULedgeGrabState::DeterminePlayerAction(float deltaTime) 
{	
	switch (climbType)
	{
	case LCT_UnderWaist:
		UnderWaistAction(deltaTime);
		break;
	case LCT_AboveWaist:
		AboveWaistAction(deltaTime);
		break;
	case LCT_AboveHead:
		AboveHeadAction(deltaTime);
		break;
	case LCT_Airborne:
		AirborneAction(deltaTime);
		break;
	}
}
float ULedgeGrabState::LiftPlayerUp(float deltaTime)
{
	climbTimer -= deltaTime;
	const float frac = 1.f - FMath::Clamp(climbTimer / climbDuration, 0.f, 1.f);
	const float newHeight = frac * climbDistance;

	player->collider->SetRelativeLocation(FVector(
		startLoc.X,
		startLoc.Y,
		startLoc.Z + newHeight));

	return frac;
}

void ULedgeGrabState::UnderWaistAction(float deltaTime)
{
	const float frac = LiftPlayerUp(deltaTime);

	if (frac >= 0.22f && frac <= 0.44f)
	{
		const float newCamHeight = FMath::Sin(frac * 14.3f) * player->camera->underWaistDip;

		player->camera->SetRelativeLocation(FVector(0, 0, 60.f + newCamHeight));
	}
	
	if (frac >= 1.f)
	{
		PushPlayerForward();
	}
}

void ULedgeGrabState::AboveWaistAction(float deltaTime)
{

}

void ULedgeGrabState::AboveHeadAction(float deltaTime)
{

}

void ULedgeGrabState::AirborneAction(float deltaTime)
{

}

void ULedgeGrabState::PushPlayerForward()
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