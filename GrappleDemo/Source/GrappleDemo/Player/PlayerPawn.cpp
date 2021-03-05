#include "PlayerPawn.h"
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

	HandleStandUp(deltaTime);

	UpdateCameraFOVState();
	UpdateCameraFOV(deltaTime);
	UpdateCameraShakeState();
	UpdateCameraShake(deltaTime, shakeAmp, shakeFreq);


	CastGrappleRaycast();	
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

bool APlayerPawn::CastGrappleRaycast()
{
	FVector Start = playerCamera->GetComponentLocation();
	FVector End = playerCamera->GetForwardVector() * grappleComponent->grappleFireRange + Start;
	FCollisionQueryParams CollisionParams;

	// ignore collision with player
	CollisionParams.AddIgnoredActor(this);

	// called if they raycast hits something
	if (GetWorld()->LineTraceSingleByChannel(GrappleHitPoint, Start, End, ECC_GameTraceChannel3, CollisionParams))
	{
		AGrappleReactor* reactor = Cast<AGrappleReactor>(GrappleHitPoint.Actor);
		if (IsValid(reactor))
			grappleComponent->grappleReactor = reactor;
		else
			grappleComponent->grappleReactor = nullptr;


		grappleCanAttach = true;

		return true;
	}
	else
	{
		grappleCanAttach = false;
	}

	return false;
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

#pragma endregion

#pragma region Camera Logic

void APlayerPawn::UpdateCameraFOVState()
{
	this->prevFOVState = this->fovState;

	if (this->playerCollider->GetPhysicsLinearVelocity().Size() >= FOVVelocityThreshold)
		this->fovState = Active;
	else
		this->fovState = Passive;

	//If the player swapped states we need to signal that the FOV needs to lerp
	if (this->prevFOVState != this->fovState)
		fovTransition = true;

}

void APlayerPawn::UpdateCameraFOV(float deltaTime)
{
	if (fovTransition)
	{
		float frac = fovTimer / FOVTransitionTime;
		float newFOV;

		switch (this->fovState)
		{
		case Passive:
			newFOV = FMath::Lerp(FOVActive, FOVPassive, frac);
			playerCamera->FieldOfView = newFOV;
			break;
		case Active:
			newFOV = FMath::Lerp(FOVPassive, FOVActive, frac);
			playerCamera->FieldOfView = newFOV;
			break;
		}

		fovTimer += deltaTime;

		if (frac >= 1)
		{
			fovTransition = false;
			fovTimer = 0;
		}
	}
}

void APlayerPawn::UpdateCameraShakeState()
{
	if (state == UWalkState::GetInstance() || state == URunState::GetInstance())
	{
		if (this->bIsGrounded && !moveVector.IsNearlyZero(0.05f))
		{
			//We should only apply shake when the player is both grounded and moving
			if (shakeState != Shaking)
			{
					blendingIn = true;
					shakeState = Shaking;
					shakeOffset = 0;
					shakeInTimer = 0;
					shakeOutTimer = 0;

					
			}
		}

		else if(shakeState == Shaking && !blendingOut)
		{
			blendingIn = false;
			blendingOut = true;
			shakeStartOffset = shakeHeight;
		}
	}

	else if(shakeState == Shaking && !blendingOut)
	{
		blendingIn = false;
		blendingOut = true;
		shakeStartOffset = shakeHeight;
	}

	if (state == UWalkState::GetInstance())
	{
		shakeAmp = passiveAmplitude;
		shakeFreq = passiveFrequency;
	}

	else if (state == URunState::GetInstance())
	{
		shakeAmp = activeAmplitude;
		shakeFreq = activeFrequency;
	}
}

void APlayerPawn::UpdateCameraShake(const float deltaTime, const float amplitude, const float freq)
{
	if (shakeState == Shaking)
	{
		if (blendingIn && shakeInTimer < shakeBlendInTime)
		{
			//If we're blending in, tick the timer.
			shakeInTimer += deltaTime;
		}

		else if (shakeInTimer >= shakeBlendInTime)
		{
			//Blend in complete
			blendingIn = false;
			shakeInTimer = 0;
		}

		if (blendingOut && shakeOutTimer < shakeBlendInTime)
		{
			//If we're blending out, tick the timer and lerp offset towards 0
			shakeOutTimer += deltaTime;

			float frac = shakeOutTimer / shakeBlendOutTime;
			frac = FMath::Clamp(frac, 0.f, 1.f);
			
			shakeHeight = FMath::Lerp(shakeStartOffset, 0.f, frac);
		}

		else if (shakeOutTimer >= shakeBlendInTime)
		{
			//Blend out complete
			blendingOut = false;
			shakeState = Stopped;
			shakeOutTimer = 0;
		}

		else
		{
			//Calculate the blend strength
			const float in = shakeInTimer / shakeBlendInTime;
	
			float blendStrength = 1.f;

			if (blendingIn)
				blendStrength = in;

			shakeOffset += (deltaTime * blendStrength * freq);

			shakeHeight = FMath::Sin(shakeOffset) * amplitude;
		}

		playerCamera->SetRelativeLocation(FVector(0, 0, standingCameraHeight + shakeHeight));
	}
}

#pragma endregion
