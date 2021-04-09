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
	this->lookPoint = player->collider->LedgeTop.ImpactPoint;
	this->bobTimer = 0;
	this->pushTimer = 0;
	this->bNeedsToBob = false;
	this->bNeedsToPush = false;

	FVector lookDir = lookPoint - player->camera->GetComponentLocation();
	lookDir.Normalize();
	goalRot = lookDir;

	if (player->bGrounded)
	{
		if (lookPoint.Z > player->camera->GetComponentLocation().Z)
		{
			climbType = LCT_AboveHead;
			bNeedsToTurn = true;
		}
				
		else if (lookPoint.Z < startLoc.Z)
			climbType = LCT_UnderWaist;

						
		else
			climbType = LCT_AboveWaist;			
	}

	else
	{
		climbType = LCT_Airborne;
		bNeedsToTurn = true;
	}
}

void ULedgeGrabState::DeterminePlayerAction(float deltaTime) 
{	
	//BobCamera(deltaTime);


	if (bNeedsToPush)
		PushPlayerForward(deltaTime);
	else
		LiftPlayerUp(deltaTime);

	/*else
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
	}*/
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

void ULedgeGrabState::UnderWaistAction(float deltaTime)
{
	LiftPlayerUp(deltaTime);
}

void ULedgeGrabState::AboveWaistAction(float deltaTime)
{
	LiftPlayerUp(deltaTime);
}

//TODO refactor these else ifs into an enum state switch
void ULedgeGrabState::AboveHeadAction(float deltaTime)
{
	if (bNeedsToTurn && !bNeedsToBob)
	{
		if (!player->camera->GetForwardVector().Equals(goalRot))
		{
			const FVector camForward = player->camera->GetForwardVector();
			const FRotator stepRotation = FMath::VInterpNormalRotationTo(camForward, goalRot, deltaTime, player->camera->ledgeTurnSpeed).Rotation();
		
			player->camera->SetWorldRotation(stepRotation);
		}

		else 
		{
			bNeedsToTurn = false;
		}
	}

	else if(!bNeedsToTurn)
	{
		LiftPlayerUp(deltaTime);

		if (player->camera->GetComponentLocation().Z >= lookPoint.Z)
		{
			const FVector camForward = player->camera->GetForwardVector();
			const FRotator stepRotation = FMath::VInterpNormalRotationTo(camForward, -ledge.ImpactNormal, deltaTime, player->camera->ledgeTurnSpeed).Rotation();

			player->camera->SetWorldRotation(stepRotation);

			if (player->camera->GetForwardVector().Equals(-ledge.ImpactNormal))
			{
				bNeedsToBob = true;
			}
		}

		if (player->collider->GetComponentLocation().Z > liftHeight)
		{
			FVector playerLoc = player->collider->GetComponentLocation();

			player->collider->SetRelativeLocation(FVector(
				playerLoc.X,
				playerLoc.Y,
				liftHeight
			));

			PushPlayerForward(deltaTime);
		}
	}	
}

void ULedgeGrabState::AirborneAction(float deltaTime)
{
	AboveHeadAction(deltaTime);
}

void ULedgeGrabState::PushPlayerForward(float deltaTime)
{
	bNeedsToPush = true;

	FVector playerLoc = player->collider->GetComponentLocation();
	FVector targetPoint = lookPoint + (FVector::UpVector * player->standHeight);

	if (!playerLoc.Equals(targetPoint, 1.f))
	{
		pushTimer += deltaTime;
		const FVector playerXY = FVector(startLoc.X, startLoc.Y, lookPoint.Z);
		const float frac = FMath::Clamp(pushTimer / .1f, 0.f, 1.f);

		const FVector newLoc = FMath::Lerp(playerXY, lookPoint, frac);

		player->collider->SetRelativeLocation(newLoc + FVector::UpVector * player->standHeight);
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

void ULedgeGrabState::BobCamera(float deltaTime)
{
	if (bNeedsToBob)
	{
		bobTimer += (deltaTime * player->camera->ledgeDipSpeed);

		const float period = FMath::Sin(bobTimer * 2.f * PI) * deltaTime * player->camera->ledgeDipAmplitude;
		const FRotator addRot = FRotator(-period, 0, 0);

		player->camera->AddLocalRotation(addRot);

		if (bobTimer >= 1.f)
		{
			bNeedsToBob = false;
		}
	}
}