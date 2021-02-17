#include "PlayerPawn.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(MyLog);

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

	//grappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("Grapple"));
	//grappleComponent->AttachTo(playerCamera);
	//grappleComponent->AttachToComponent(playerCamera, FAttachmentTransformRules(EAttachmentRule::KeepRelative, false));
	//AttachTo is deprecated
}

void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	playerCollider->SetRelativeLocation(FVector::ZeroVector);
	playerCamera->SetRelativeLocation(FVector::ZeroVector);
}

void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckIfGrounded();

	PlayerLook(lookVector, DeltaTime);
	PlayerMove(moveVector, DeltaTime);

	ClampPlayerMoveSpeed();
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
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpPress);
	InputComponent->BindAction("Jump", IE_Pressed, this, &APlayerPawn::JumpRelease);
}

void APlayerPawn::MoveInputX(float value) { moveVector.X = value; }
void APlayerPawn::MoveInputY(float value) { moveVector.Y = value; }
void APlayerPawn::LookInputX(float value) { lookVector.X = value; }
void APlayerPawn::LookInputY(float value) { lookVector.Y = value; }
void APlayerPawn::JumpPress() { PlayerJump(); } //hacky
void APlayerPawn::JumpRelease() { /*Do nothing for now*/; }

#pragma endregion

#pragma region Game Logic

void APlayerPawn::PlayerMove(FVector2D rawInputVector, float deltaTime)
{
	if (!rawInputVector.IsZero())
	{
		FVector relativeInputVector = ConvertInputRelativeToCamera(rawInputVector);
		
		//multiplying by 100 so the designer values aren't so massive
		playerCollider->AddForce(relativeInputVector * acceleration * deltaTime * 100, NAME_None, true); //Set to false if you want player mass to matter
	}
}

void APlayerPawn::PlayerLook(FVector2D inputVector, float deltaTime)
{
	if (!inputVector.IsZero())
	{
		FRotator camRotation = playerCamera->GetRelativeRotation();

		camRotation.Yaw += (inputVector.X * lookSpeed * deltaTime);
		camRotation.Pitch = FMath::Clamp(camRotation.Pitch + (inputVector.Y * lookSpeed * deltaTime), viewLookBounds.X, viewLookBounds.Y);

		playerCamera->SetRelativeRotation(camRotation);
	}
}

void APlayerPawn::PlayerJump()
{
	APlayerPawn::CastRaycast();
	if (groundState == Grounded) 
	{
		//multiplying by 100 so the designer values aren't so massive
		playerCollider->AddForce(FVector::UpVector * jumpForce * 100, NAME_None, true); 
	}
}

FVector APlayerPawn::ConvertInputRelativeToCamera(FVector2D vectorToConvert)
{
	//Kill Z component and normalize vectors
	FVector camForwardVector = playerCamera->GetForwardVector();
	FVector camRightVector = playerCamera->GetRightVector();
	camForwardVector.Z = 0;
	camRightVector.Z = 0;
	camForwardVector.Normalize(0.0001);
	camRightVector.Normalize(0.0001);

	//Multiply input axis values by the cameras look vectors
	FVector relativeVector = (camForwardVector * vectorToConvert.Y) + (camRightVector * vectorToConvert.X);
	relativeVector.Normalize(0.0001);

	return relativeVector;
}

void APlayerPawn::ClampPlayerMoveSpeed()
{
	FVector movementVelocity = playerCollider->GetPhysicsLinearVelocity();
	movementVelocity.Z = 0;
	movementVelocity = movementVelocity.GetClampedToMaxSize(maxMoveSpeed);

	FVector fallVelocity = playerCollider->GetPhysicsLinearVelocity();
	fallVelocity.X = 0;
	fallVelocity.Y = 0;
	fallVelocity = fallVelocity.GetClampedToMaxSize(maxFallSpeed);

	FVector playerVelocity = movementVelocity + fallVelocity;

	playerCollider->SetPhysicsLinearVelocity(playerVelocity);
}

void APlayerPawn::CheckIfGrounded()
{
	FHitResult hit;

	FVector rayOrigin = playerCollider->GetRelativeLocation() - playerCollider->GetScaledCapsuleHalfHeight();
	FVector rayDest = rayOrigin + (FVector::DownVector * minDistanceToBeGrounded);

	FCollisionQueryParams param;
	param.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(hit, rayOrigin, rayDest, ECC_Visibility, param);

	if (bHit)
		groundState = Grounded;
	else
		groundState = Airborne;
}

void APlayerPawn::CastRaycast()
{
	FHitResult outHit;
	FVector Start = playerCamera->GetForwardVector();
	// TODO change number to cable length
	FVector End = playerCamera->GetForwardVector() * 99999 + Start;
	FCollisionQueryParams CollisionParams;

	DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 5);

	if (ActorLineTraceSingle(outHit, Start, End, ECC_WorldStatic, CollisionParams))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *outHit.GetComponent()->GetName()));
	}
}

#pragma endregion

