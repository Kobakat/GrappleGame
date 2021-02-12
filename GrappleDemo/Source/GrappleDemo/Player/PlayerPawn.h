#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/CapsuleComponent.h"
#include "PlayerPawn.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MyLog, Log, All);

#pragma region State Enums
UENUM()
enum EMoveState
{
	/*No input is being recieved*/
	Idle UMETA(DisplayName = "Idle"),

	/*Player is moving normally*/
	Walking UMETA(DisplayName = "Walking"),

	/*Player is moving while holding run key*/
	Running UMETA(DisplayName = "Running"),

	/*Player is currently sliding forward*/
	Sliding UMETA(DisplayName = "Sliding")
};

UENUM()
enum EGroundState
{
	/*Player is standing on solid ground*/
	Grounded UMETA(DisplayName = "Grounded"),

	/*Player is under the effects of gravity*/
	Airborne UMETA(DisplayName = "Airborne")
};
#pragma endregion

UCLASS()
class GRAPPLEDEMO_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerPawn();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

private:

#pragma region UPROPs

	UPROPERTY(EditAnywhere, Category = "Camera")
		UCameraComponent* playerCamera;
	
	UPROPERTY(EditAnywhere, Category = "Player Collider")
		UCapsuleComponent* playerCollider;

	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float acceleration;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float maxMoveSpeed;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float jumpForce;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float maxFallSpeed;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float lookSpeed;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		float minDistanceToBeGrounded;
	UPROPERTY(EditAnywhere, Category = "PlayerStats")
		FVector2D viewLookBounds;

	UPROPERTY(VisibleAnywhere, Category = "PlayerStats")
		TEnumAsByte<EMoveState> moveState;
	UPROPERTY(VisibleAnywhere, Category = "PlayerStats")
		TEnumAsByte<EGroundState> groundState;

#pragma endregion

#pragma region Functions

	FVector2D moveVector;
	FVector2D lookVector;

	void MoveInputX(float value);
	void MoveInputY(float value);
	void LookInputX(float value);
	void LookInputY(float value);
	void JumpPress();
	void JumpRelease();

	void PlayerMove(FVector2D rawInputVector, float deltaTime);
	void PlayerLook(FVector2D lookVector, float deltaTime);
	void PlayerJump();

	FVector ConvertInputRelativeToCamera(FVector2D vectorToConvert);
	void ClampPlayerMoveSpeed();
	void CheckIfGrounded();

#pragma endregion
};


