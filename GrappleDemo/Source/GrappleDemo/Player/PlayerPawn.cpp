#include "PlayerPawn.h"

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
	raycastDistance = 5000;
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
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (this->stateMachine != nullptr) 
	{
		stateMachine->Tick();
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
		grappleInputBuffered = true;
	}
}
void APlayerPawn::InstantReelPress() { instantReelInputBuffered = true; }
bool APlayerPawn::IsTryingToGrapple()
{
	if (grappleInputBuffered)
	{
		grappleInputBuffered = false;
		return true;
	}
	else
		return false;
}
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
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, true);
		
		grappleComponent->Attach(outHit->Location);
		/*if (outHit->GetActor() == grappleReactor)
		{
			grappleReactor = Cast<outHit->GetActor(AGrappleReactor)>(outhit->GetActor());
			return true
		}*/

		return true;
	}

	return false;
}