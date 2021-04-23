#include "PlayerPawn.h"
#include "Components/ChildActorComponent.h"
#include "../GrappleInteractions/GrappleReactor.h"
#include "../LevelPreview/LevelPreviewPawn.h"
#include "Kismet/GameplayStatics.h"

#pragma region Unreal Event Functions

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	collider = CreateDefaultSubobject<UPlayerCapsule>(TEXT("Collider"));
	collider->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	camera = CreateDefaultSubobject<Ucringetest>(TEXT("Player Camera"));
	camera->AttachToComponent(collider, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
}

bool APlayerPawn::GetHasGrapple()
{
	return hasGrapple;
}

void APlayerPawn::SetHasGrapple(bool HasGrapple)
{
	hasGrapple = HasGrapple;
	// Hide or reveal the gun mesh
	gun->SetHiddenInGame(!hasGrapple, true);
	// Update the rendered state of the grapple
	// This will also prevent grapple states
	// from being entered
	grappleComponent->SetIsRendered(hasGrapple);
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Ensure the grapple polyline is instantiated.
	UChildActorComponent* childActor = FindComponentByClass<UChildActorComponent>();
	if (!childActor->HasBeenCreated())
		childActor->CreateChildActor();
	GrapplePolyline = Cast<APolylineCylinderRenderer>(childActor->GetChildActor());

	// Grab the grapple gun
	grappleComponent = FindComponentByClass<UGrappleGunComponent>();
	grappleComponent->SetCastingFromComponent(camera);
	grappleComponent->IgnoredActors.Add(this);
	grappleComponent->Polyline = GrapplePolyline;
	SetHasGrapple(hasGrapple);


	this->stateMachine = NewObject<UStateMachine>();
	this->stateMachine->Initialize(this);

	LinkPreviewCamera();
}

void APlayerPawn::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


	if (this->stateMachine != nullptr) 
	{
		stateMachine->Tick(deltaTime);
	}

	collider->HandleStandUp(deltaTime);
	grappleCanAttach = grappleComponent->GetCanAttach();

	collider->previousVelocity = collider->GetPhysicsLinearVelocity();

	if (collider->previousVelocity.Z < lastFallingSpeed)
		lastFallingSpeed = collider->previousVelocity.Z;


	bPreviousGrounded = bGrounded;

	// Handle input buffering
	if (SwingBuffered || InstantBuffered)
	{
		if (GetWorld()->GetTimeSeconds() - BufferedTime
			< grappleComponent->InputBufferSeconds)
		{
			if (grappleComponent->RunBufferCheck())
			{
				SwingBuffered = InstantBuffered = false;
				if (SwingBuffered)
					SetState(UGrappleAirborneState::GetInstance());
				else
					SetState(UGrappleInstantReelState::GetInstance());
				grappleComponent->Attach();
			}
		}
		else
			SwingBuffered = InstantBuffered = false;
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
	if (grappleComponent->RunBufferCheck())
	{
		SetState(UGrappleAirborneState::GetInstance());
		grappleComponent->Attach();
	}
	else if (stateMachine->state == UGrappleAirborneState::GetInstance())
	{
		// This detaches the grapple if the player clicks
		// again and there is nothing within grapple range
		SetState(UWalkState::GetInstance());
	}
	else if (stateMachine->state == UGrappleInstantReelState::GetInstance())
	{
		// This detaches the grapple if the player clicks
		// again and there is nothing within grapple range
		SetState(UGrappleAirborneState::GetInstance());
	}
	else
	{
		// Otherwise buffer the input
		BufferedTime = GetWorld()->GetTimeSeconds();
		SwingBuffered = true;
		InstantBuffered = false;
	}
}
void APlayerPawn::ShootReleaseRelease() { tryingToGrapple = false; }

void APlayerPawn::InstantReelPress()
{
	if (grappleComponent->RunBufferCheck())
	{
		SetState(UGrappleInstantReelState::GetInstance());
		grappleComponent->Attach();
	}
	else if (stateMachine->state == UGrappleAirborneState::GetInstance()
		|| stateMachine->state == UGrappleInstantReelState::GetInstance())
	{
		// This detaches the grapple if the player clicks
		// again and there is nothing within grapple range
		SetState(UWalkState::GetInstance());
	}
	else
	{
		// Otherwise buffer the input
		BufferedTime = GetWorld()->GetTimeSeconds();
		SwingBuffered = false;
		InstantBuffered = true;
	}
}

#pragma endregion

#pragma region Logic

void APlayerPawn::SetState(UState* newState) 
{
	stateMachine->SetState(newState);
}

void APlayerPawn::LinkPreviewCamera()
{
	AActor* uncastedActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALevelPreviewPawn::StaticClass());

	if (uncastedActor != nullptr)
	{
		ALevelPreviewPawn* castedActor = Cast<ALevelPreviewPawn>(uncastedActor);
		castedActor->playerPawn = this;
		
		AController* controller = GetController();
		controller->UnPossess();
		controller->Possess(castedActor);
		castedActor->StartPreview();
	}
}
#pragma endregion

