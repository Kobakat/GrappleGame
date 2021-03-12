#include "PlayerCamera.h"
#include "PlayerPawn.h"

UPlayerCamera::UPlayerCamera() { PrimaryComponentTick.bCanEverTick = true; }
void UPlayerCamera::BeginPlay() 
{ 
	this->player = Cast<APlayerPawn>(GetOwner());
	this->initialHeight = GetRelativeLocation().Z;
}

void UPlayerCamera::TickComponent(float deltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UpdateFOVState();
	UpdateFOV(deltaTime);
	UpdateShakeState();
	UpdateShake(deltaTime, shakeAmp, shakeFreq);
}

void UPlayerCamera::UpdateFOVState()
{
	/*this->prevFOVState = this->fovState;

	if (player->state == URunState::GetInstance()
		|| player->state == URunSlideState::GetInstance())
		this->fovState = Active;
	else
		this->fovState = Passive;

	//If the player swapped states we need to signal that the FOV needs to lerp
	if (this->prevFOVState != this->fovState)
		fovTransition = true;*/
}

void UPlayerCamera::UpdateFOV(const float deltaTime)
{
	/*if (fovTransition)
	{
		const float frac = fovTimer / FOVTransitionTime;
		float newFOV;

		switch (this->fovState)
		{
		case Passive:
			newFOV = FMath::Lerp(this->FieldOfView, FOVPassive, frac);
			this->FieldOfView = newFOV;
			break;
		case Active:
			newFOV = FMath::Lerp(this->FieldOfView, FOVActive, frac);
			this->FieldOfView = newFOV;
			break;
		}

		fovTimer += deltaTime;

		if (frac >= 1)
		{
			fovTransition = false;
			fovTimer = 0;
		}
	}*/
}

void UPlayerCamera::UpdateShakeState()
{
	/*if (player->state == UWalkState::GetInstance() || player->state == URunState::GetInstance())
	{
		if (player->bIsGrounded && !player->moveVector.IsNearlyZero(0.05f))
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

		else if (shakeState == Shaking && !blendingOut)
		{
			blendingIn = false;
			blendingOut = true;
			shakeStartOffset = shakeHeight;
		}
	}

	else if (shakeState == Shaking && !blendingOut)
	{
		blendingIn = false;
		blendingOut = true;
		shakeStartOffset = shakeHeight;
	}

	if (player->state == UWalkState::GetInstance())
	{
		shakeAmp = passiveAmplitude;
		shakeFreq = passiveFrequency;
	}

	else if (player->state == URunState::GetInstance())
	{
		shakeAmp = passiveAmplitude;
		shakeFreq = passiveAmplitude;
	}*/
}

void UPlayerCamera::UpdateShake(const float deltaTime, const float amplitude, const float freq)
{
	/*if (shakeState == Shaking)
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

		this->SetRelativeLocation(FVector(0, 0, initialHeight + shakeHeight));
	}*/
}