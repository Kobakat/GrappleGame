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
	playerCollider->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	playerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	playerCamera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	grappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("Grapple"));
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	grappleComponent->AttachToComponent(grappleStart, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	this->stateMachine = NewObject<UStateMachine>();
	this->stateMachine->Initialize(this);
	this->playerCollider->SetCapsuleHalfHeight(standingPlayerHeight);
	this->playerCamera->SetRelativeLocation(FVector(0, 0, 0));
	this->standUpTimer = 0;

	// This is done in begin play because otherwise it
	// shows up in the editor and acts kinda janky.
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

	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Yellow, FString::SanitizeFloat(this->GetVelocity().Size()));
	}*/

	// Updates Camera FOV based on player velocity
	UpdateCameraFOV();

	// Checks if the player is looking at a grappable object
	CastGrappleRaycast();
	HandleStandUp(deltaTime);
}

#pragma endregion

#pragma region Input Initialization

void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("MoveX", this, &APlayerPawn::MoveInputX);
	InputComponent->BindAxis("MoveY", this, &APlayerPawn::MoveInputY);
	InputComponent->BindAxis("LookX", this, &APlayerPawn::LookInputX);
	InputComponent->BindAxis("LookY", this, &APlayerPawn::LookInputY);
	InputComponent->BindAxis("IncrementalReelUnreel", this, &APlayerPawn::ReelInputAxis);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpPress);
	InputComponent->BindAction("Jump", IE_Released, this, &APlayerPawn::JumpRelease);
	InputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerPawn::RunPress);
	InputComponent->BindAction("Sprint", IE_Released, this, &APlayerPawn::RunRelease);
	InputComponent->BindAction("CrouchSlide", IE_Pressed, this, &APlayerPawn::CrouchSlidePress);
	InputComponent->BindAction("CrouchSlide", IE_Released, this, &APlayerPawn::CrouchSlideRelease);
	InputComponent->BindAction("ShootRelease", IE_Pressed, this, &APlayerPawn::ShootReleasePress);
	InputComponent->BindAction("ShootRelease", IE_Released, this, &APlayerPawn::ShootReleaseRelease);
	InputComponent->BindAction("InstantReel", IE_Pressed, this, &APlayerPawn::InstantReelPress);
}

void APlayerPawn::MoveInputX(float value) { moveVector.X = value; }
void APlayerPawn::MoveInputY(float value) { moveVector.Y = value; }
void APlayerPawn::LookInputX(float value) { lookVector.X = value; }
void APlayerPawn::LookInputY(float value) { lookVector.Y = value; }
void APlayerPawn::ReelInputAxis(float value) { reelingAxis = value; }
void APlayerPawn::JumpPress() { tryingToJump = true; }
void APlayerPawn::JumpRelease() { tryingToJump = false; }
void APlayerPawn::RunPress() { tryingToSprint = true; }
void APlayerPawn::RunRelease() { tryingToSprint = false; }
void APlayerPawn::CrouchSlidePress() { tryingToCrouch = true; }
void APlayerPawn::CrouchSlideRelease() { tryingToCrouch = false; }
void APlayerPawn::ShootReleasePress() 
{
	if (ShootGrapple())
	{
		SetState(UGrappleAirborneState::GetInstance());
	}
	else if (stateMachine->state == UGrappleAirborneState::GetInstance())
	{
		// This detaches the grapple if the player clicks
		// again and there is nothing within grapple range.
		SetState(UWalkState::GetInstance());
	}
}
void APlayerPawn::ShootReleaseRelease() { tryingToGrapple = false; }

void APlayerPawn::InstantReelPress()
{
	if (ShootGrapple())
	{
		SetState(UGrappleInstantReelState::GetInstance());
	}
}

#pragma endregion

#pragma region Logic

void APlayerPawn::SetState(UState* newState) 
{
	stateMachine->SetState(newState);
}

void APlayerPawn::HandleStandUp(float deltaTime)
{
	if (bNeedsToStand)
	{
		if (playerCollider->GetScaledCapsuleHalfHeight() < standingPlayerHeight)
		{
			float frac = standUpTimer / crouchTransitionTime;
			float newCapHeight = FMath::Lerp(crouchSlidePlayerHeight, standingPlayerHeight, frac);
			float newCamHeight = FMath::Lerp(crouchSlideCameraHeight, standingCameraHeight, frac);

			playerCollider->SetCapsuleHalfHeight(newCapHeight);
			playerCamera->SetRelativeLocation(FVector(0, 0, newCamHeight));

			standUpTimer += deltaTime;

		}

		else
		{
			bNeedsToStand = false;
			standUpTimer = 0;
		}
	}
}

void APlayerPawn::CastGrappleRaycast()
{
	// Cast from the camera out to the grapple fire range.
	FVector Start = playerCamera->GetComponentLocation();
	FVector End = Start + playerCamera->GetForwardVector() * grappleComponent->grappleFireRange;
	// Ignore collision with player.
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	// Is there a grapple surface within range?
	if (GetWorld()->LineTraceSingleByChannel(GrappleHitPoint, Start, End, ECC_GameTraceChannel3, CollisionParams))
	{
		// Check for a grapple blocker that may be closer.
		FHitResult blockerHitPoint;
		bool isBlocked = false;
		if (GetWorld()->LineTraceSingleByChannel(blockerHitPoint, Start, End, ECC_GameTraceChannel4, CollisionParams))
		{
			if ((blockerHitPoint.Location - Start).SizeSquared()
				< (GrappleHitPoint.Location - Start).SizeSquared())
			{
				isBlocked = true;
			}
		}
		// Set the state of attachability based on whether we are
		// currently blocked.
		if (isBlocked)
			grappleCanAttach = false;
		else
		{
			grappleCanAttach = true;
			// Document a grapple reactor if it exists on the actor.
			AGrappleReactor* reactor = Cast<AGrappleReactor>(GrappleHitPoint.Actor);
			if (IsValid(reactor))
				grappleComponent->grappleReactor = reactor;
			else
				grappleComponent->grappleReactor = nullptr;
		}
	}
	else
		grappleCanAttach = false;
}

bool APlayerPawn::ShootGrapple()
{
	if (grappleCanAttach)
	{
		// Attaches the cable component to the grappable object
		grappleComponent->Attach(GrappleHitPoint.ImpactPoint, GrappleHitPoint.GetActor());
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
#pragma endregion
