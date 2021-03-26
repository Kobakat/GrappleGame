#include "PlayerPawn.h"
#include "Components/ChildActorComponent.h"
#include "../GrappleInteractions/GrappleReactor.h"

#pragma region Unreal Event Functions

APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	bUseControllerRotationYaw = false;

	collider = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collider"));
	collider->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));

	camera = CreateDefaultSubobject<Ucringetest>(TEXT("Player Camera"));
	camera->AttachToComponent(RootComponent, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	// Grab the grapple gun
	grappleComponent = FindComponentByClass<UGrappleGunComponent>();
	grappleComponent->SetCastingFromComponent(camera);
	grappleComponent->IgnoredActors.Add(this);

	// Ensure the grapple polyline is instantiated.
	UChildActorComponent* childActor = FindComponentByClass<UChildActorComponent>();
	if (!childActor->HasBeenCreated())
		childActor->CreateChildActor();
	GrapplePolyline = Cast<APolylineCylinderRenderer>(childActor->GetChildActor());

	this->stateMachine = NewObject<UStateMachine>();
	this->stateMachine->Initialize(this);
	this->collider->SetRelativeScale3D(FVector(1, 1, standHeightScale));

	this->bounds = CalculateBounds();
}

void APlayerPawn::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


	if (this->stateMachine != nullptr) 
	{
		stateMachine->Tick(deltaTime);
	}

	HandleStandUp(deltaTime);

	grappleCanAttach = grappleComponent->GetCanAttach();

	bPreviousGround = bIsGrounded;
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
	if (grappleComponent->GetCanAttach())
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
}
void APlayerPawn::ShootReleaseRelease() { tryingToGrapple = false; }

void APlayerPawn::InstantReelPress()
{
	if (grappleComponent->GetCanAttach())
	{
		SetState(UGrappleInstantReelState::GetInstance());
		grappleComponent->Attach();
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
		FCollisionQueryParams param;
		param.AddIgnoredActor(this);

		FCollisionShape box = FCollisionShape::MakeBox(bounds);

		bool bHitCeiling = GetWorld()->SweepSingleByChannel(
			CrouchHitPoint,
			GetActorLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
			GetActorLocation() + FVector(0, 0, bounds.Z) + FVector::UpVector,
			FQuat::Identity,
			ECC_Visibility,
			box,
			param);

		

		if (bHitCeiling) 
		{
			SetState(UCrouchState::GetInstance());
		}

		else 
		{
			const float currentScale = collider->GetRelativeScale3D().Z;
			gun->SetRelativeScale3D(FVector(1, 1, 1.f / currentScale));
			if (currentScale != standHeightScale)
			{
				standUpTimer += deltaTime;
				const float frac = FMath::Clamp(standUpTimer / crouchTransitionTime, 0.f, 1.f);
				const float newScale = FMath::Lerp(currentScale, standHeightScale, frac);

				collider->SetRelativeScale3D(FVector(1, 1, newScale));
			}

			else
			{
				bNeedsToStand = false;
				standUpTimer = 0;
			}
		}	
	}
}

#pragma endregion

FVector APlayerPawn::CalculateBounds() 
{
	return collider->GetStaticMesh()->GetBounds().BoxExtent;
}

