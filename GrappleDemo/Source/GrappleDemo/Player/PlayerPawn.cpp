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
	this->stateMachine = NewObject<UStateMachine>();
	this->stateMachine->Initialize(this);
	this->playerCollider->SetCapsuleHalfHeight(standingPlayerHeight);
	this->playerCamera->SetRelativeLocation(FVector(0, 0, standingCameraHeight));


	// This is done in begin play because otherwise it
	// shows up in the editor and acts kinda janky.
	grappleComponent->AttachToComponent(grappleStart, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
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
	if (CastGrappleRaycast())
	{
		SetState(UGrappleAirborneState::GetInstance());
	}
}
void APlayerPawn::ShootReleaseRelease() { tryingToGrapple = false; }

void APlayerPawn::InstantReelPress()
{
	if (CastGrappleRaycast())
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

bool APlayerPawn::CastGrappleRaycast()
{
	FHitResult* outHit = new FHitResult();

	FVector Start = playerCamera->GetComponentLocation();
	FVector End = playerCamera->GetForwardVector() * grappleComponent->grappleFireRange + Start;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

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

#pragma endregion