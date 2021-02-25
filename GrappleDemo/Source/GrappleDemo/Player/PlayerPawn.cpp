#include "PlayerPawn.h"
#include "Engine/StaticMesh.h"
#include "../GrappleInteractions/GrappleReactor.h"

#pragma region Unreal Event Functions
APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	playerCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collider"));
	playerCollider->AttachTo(RootComponent);

	playerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	playerCamera->AttachTo(RootComponent);
	//AttachTo is deprecated
	
	grappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("Grapple"));
	
	grappleComponent->SetHiddenInGame(true);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	this->stateMachine = NewObject<UStateMachine>();
	this->stateMachine->Initialize(this);
	this->playerCollider->SetCapsuleHalfHeight(standingPlayerHeight);
	this->playerCamera->SetRelativeLocation(FVector(0, 0, standingCameraHeight));
	raycastDistance = grappleComponent->maxGrappleLength;

	// This is done in begin play because otherwise it
	// shows up in the editor and acts kinda janky.
	grappleComponent->AttachToComponent(grappleStart, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	grappleComponent->SetHiddenInGame(true);
	grappleComponent->NumSegments = 10;
	grappleComponent->NumSides = 8;
	grappleComponent->SolverIterations = 4;
}

void APlayerPawn::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (this->stateMachine != nullptr) 
	{
		stateMachine->Tick(deltaTime);
	}

	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("StateMachine was set to a nullptr!!!"));
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Yellow, FString::SanitizeFloat(this->GetVelocity().Size()));
	}
	UpdateCameraFOV();
}

#pragma endregion

#pragma region Input Initialization

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Basic movement bindings.
	InputComponent->BindAxis("MoveX", this, &APlayerPawn::MoveInputX);
	InputComponent->BindAxis("MoveY", this, &APlayerPawn::MoveInputY);
	InputComponent->BindAxis("LookX", this, &APlayerPawn::LookInputX);
	InputComponent->BindAxis("LookY", this, &APlayerPawn::LookInputY);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpPress);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerPawn::JumpRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerPawn::RunPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &APlayerPawn::RunRelease);
	InputComponent->BindAction("CrouchSlide", IE_Pressed, this, &APlayerPawn::CrouchSlidePress);
	InputComponent->BindAction("CrouchSlide", IE_Released, this, &APlayerPawn::CrouchSlideRelease);
	// Grapple movement bindings.
	InputComponent->BindAxis("IncrementalReelUnreel", this, &APlayerPawn::ReelInputAxis);
	InputComponent->BindAction("ShootRelease", IE_Pressed, this, &APlayerPawn::ShootReleasePress);
	InputComponent->BindAction("ShootRelease", IE_Released, this, &APlayerPawn::ShootReleaseRelease);
	InputComponent->BindAction("InstantReel", IE_Pressed, this, &APlayerPawn::InstantReelPress);
}

void APlayerPawn::MoveInputX(float value) { moveVector.X = value; }
void APlayerPawn::MoveInputY(float value) { moveVector.Y = value; }
void APlayerPawn::LookInputX(float value) { lookVector.X = value; }
void APlayerPawn::LookInputY(float value) { lookVector.Y = value; }
void APlayerPawn::JumpPress() { tryingToJump = true; }
void APlayerPawn::JumpRelease() { tryingToJump = false; }
void APlayerPawn::RunPress() { tryingToSprint = true; }
void APlayerPawn::RunRelease() { tryingToSprint = false; }
void APlayerPawn::CrouchSlidePress() { tryingToCrouch = true; }
void APlayerPawn::CrouchSlideRelease() { tryingToCrouch = false; }

void APlayerPawn::ReelInputAxis(float value) { reelingAxis = value; }
void APlayerPawn::ShootReleasePress() 
{
	if (CastGrappleRaycast())
	{
		stateMachine->SetState(stateMachine->grappleAirborneState);
	}
}
void APlayerPawn::InstantReelPress()
{
	if (CastGrappleRaycast())
	{
		stateMachine->SetState(stateMachine->grappleInstantReelState);
	}
}

void APlayerPawn::ShootReleaseRelease()
{
	tryingToGrapple = false;
}
//void APlayerPawn::InstantReelPress() { tryingToInstantReel = true; }
//bool APlayerPawn::IsTryingToGrapple()
//{
//	if (grappleInputBuffered)
//	{
//		grappleInputBuffered = false;
//		return true;
//	}
//	else
//		return false;
//}



bool APlayerPawn::IsTryingToInstantReel()
{
	if (instantReelInputBuffered)
	{
		instantReelInputBuffered = false;
		return true;
	}
	else
		return false;
}

#pragma endregion

bool APlayerPawn::CastGrappleRaycast()
{
	// variable holding information of raycast hit
	FHitResult* outHit = new FHitResult();

	FVector Start = playerCamera->GetComponentLocation();
	FVector End = playerCamera->GetForwardVector() * raycastDistance + Start;
	FCollisionQueryParams CollisionParams;
	// ignore collision with player
	CollisionParams.AddIgnoredActor(this);
	// called if they raycast hits something
	if (GetWorld()->LineTraceSingleByChannel(*outHit, Start, End, ECC_GameTraceChannel3, CollisionParams))
	{
		grappleComponent->Attach(outHit->GetActor()->GetActorLocation(), outHit->GetActor());
		
		AGrappleReactor* playerGrappleReactor = Cast<AGrappleReactor>(outHit->GetActor());

		if (IsValid(playerGrappleReactor))
		{
			grappleComponent->grappleReactor = playerGrappleReactor;
			
			return true;
		}
		else
		{
			grappleComponent->grappleReactor = nullptr;
		}

		return true;
	}

	return false;
}

void APlayerPawn::UpdateCameraFOV()
{
	if (this->GetVelocity().Size() >= 1000)
	{
		cameraTargetFOV = 110;
	}
	else
	{
		cameraTargetFOV = 90;
	}
	
	// TODO replace hard coded value with cameraTargetFOV
	
	playerCamera->FieldOfView = FMath::Lerp(playerCamera->FieldOfView, cameraTargetFOV, 0.03);

}