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
	InitializePositionValues();
	//InitializeCameraValues();
}

void ULedgeGrabState::StateTick(float deltaTime)
{
	DeterminePlayerAction(deltaTime);
	//DetermineCameraAction(deltaTime);
}

void ULedgeGrabState::OnStateExit()
{
	bClimbComplete = false;
}

void ULedgeGrabState::InitializePositionValues()
{
	this->startLoc = player->GetActorLocation();

	//Get the height the player is climbing to
	//TODO this will not work if the ledge moves up and down
	FVector ledgeBounds = ledge.Component->Bounds.BoxExtent;
	ledgeBounds += ledge.Actor.Get()->GetActorLocation();
	this->liftHeight = ledgeBounds.Z;

	//Get the direction to push the player
	this->pushDir = player->camera->GetForwardVector();
	this->pushDir.Z = 0;
	this->pushDir.Normalize(0.01f);
}

void ULedgeGrabState::InitializeCameraValues()
{
	FVector camLoc = player->camera->GetComponentLocation();
	FVector normalLoc = ledge.ImpactPoint;

	this->lookCorner = FVector(normalLoc.X, normalLoc.Y, liftHeight);
	this->lookDirStart = player->camera->GetForwardVector();
	this->lookDirFinal = lookCorner - camLoc;
	this->lookDirFinal.Normalize(0.01f);

	if (camLoc.Z < liftHeight)	
		this->cameraState = LCS_Turning;
	else
		this->cameraState = LCS_Done;
}

void ULedgeGrabState::DeterminePlayerAction(float deltaTime) 
{	
	if (!this->bClimbComplete)
	{
		LiftPlayerUp(deltaTime);
	}

	else
	{
		PushPlayerForward();
	}
}
void ULedgeGrabState::LiftPlayerUp(float deltaTime)
{
	FVector playerLoc = player->GetActorLocation();

	player->collider->SetRelativeLocation(FVector(
			playerLoc.X, 
			playerLoc.Y, 
			playerLoc.Z + (player->ledgeClimbSpeed * deltaTime)));

	if (playerLoc.Z > liftHeight + 5.f)//HACK hard coded buffer to make sure we don't fall too early
	{
		bClimbComplete = true;

		//Determine what kind of push we should give to the player based on their input
		if (!player->moveVector.IsNearlyZero(0.01f) && player->tryingToSprint)
			pushType = LPT_Fast;

		else if (!player->moveVector.IsNearlyZero(0.01f))
			pushType = LPT_Slow;

		else 
			pushType = LPT_Lazy;
	}
}
void ULedgeGrabState::PushPlayerForward()
{
	player->collider->SetEnableGravity(true);
	
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

void ULedgeGrabState::DetermineCameraAction(float deltaTime)
{
	switch (this->cameraState)
	{
	case LCS_Done:
		break;
	case LCS_Tilting:
		TiltCamera(deltaTime);
		break;
	case LCS_Turning:
		TurnCamera(deltaTime);
		break;
	}
}

void ULedgeGrabState::TurnCamera(float deltaTime)
{
	FVector camForward = player->camera->GetForwardVector();
	FRotator stepRotation = FMath::VInterpNormalRotationTo(camForward, lookDirFinal, deltaTime, player->camera->ledgeTurnSpeed).Rotation();
	player->camera->SetRelativeRotation(stepRotation);

	if (stepRotation.Equals(lookDirFinal.Rotation()))
		cameraState = LCS_Tilting;

	DrawDebugLine(player->GetWorld(), player->camera->GetComponentLocation(), lookCorner, FColor::Green, false, 0.01f);
}

void ULedgeGrabState::TiltCamera(float deltaTime)
{
	FVector camForward = player->camera->GetForwardVector();
	FVector pushNormal = ledge.ImpactNormal * -1;
	FRotator stepRotation = FMath::VInterpNormalRotationTo(camForward, pushNormal, deltaTime, player->camera->ledgeTiltSpeed).Rotation();
	player->camera->SetRelativeRotation(stepRotation);

	if (stepRotation.Equals(pushNormal.Rotation()))
		cameraState = LCS_Done;
}

