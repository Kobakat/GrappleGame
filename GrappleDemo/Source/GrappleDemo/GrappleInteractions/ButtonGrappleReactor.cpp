// Fill out your copyright notice in the Description page of Project Settings.


#include "ButtonGrappleReactor.h"
#include "Engine/World.h"

AButtonGrappleReactor::AButtonGrappleReactor()
{
	PrimaryActorTick.bCanEverTick = true;
}

bool AButtonGrappleReactor::GetIsPressed()
{
	return isPressed;
}

void AButtonGrappleReactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// TODO bool check on tick kinda sloppy.
	if (isPressed && IsTimer)
	{
		// Update the timer.
		float secondsElapsed = GetWorld()->TimeSeconds - startTime;
		if (secondsElapsed < TimerDuration)
		{
			// Notify the blueprint to update any animations.
			ReceiveOnTimerTick(
				(TimerDuration - secondsElapsed) / TimerDuration,
				TimerDuration - secondsElapsed);
		}
		else
		{
			// Once timer has elapsed change state.
			isPressed = false;
			ReceiveOnTimerTick(0.F, 0.F);
			ReceiveOnDeactivated();
		}
	}
}

void AButtonGrappleReactor::ApplyPullForce(const FVector force)
{
	// Check to see if the button should be triggered.
	if (!isPressed && force != FVector::ZeroVector)
	{
		isPressed = true;
		startTime = GetWorld()->TimeSeconds;
		ReceiveOnActivated();
	}
}

void AButtonGrappleReactor::Reset()
{
	// Reset the state of the button.
	if (isPressed)
	{
		isPressed = false;
		ReceiveOnDeactivated();
	}
}
