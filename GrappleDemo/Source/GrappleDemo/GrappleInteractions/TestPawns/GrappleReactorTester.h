// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"
#include "GrappleDemo/GrappleInteractions/GrappleReactor.h"
#include "GrappleReactorTester.generated.h"

UCLASS()
class GRAPPLEDEMO_API AGrappleReactorTester : public APawn
{
	GENERATED_BODY()

protected:
	// Get an inspector reference to the target to test
	UPROPERTY(EditAnywhere, Category = "Test References")
	AGrappleReactor* Reactor;
	// Get forces to apply to the target
	UPROPERTY(EditAnywhere, Category = "Test Parameters")
	FVector KeyNineForceVector;
	UPROPERTY(EditAnywhere, Category = "Test Parameters")
	FVector KeyZeroForceVector;
	// Allow the tester to use either presses or holds
	UPROPERTY(EditAnywhere, Category = "Test Parameters")
	TEnumAsByte<EInputEvent> InputMode;

public:
	// Sets default values for this pawn's properties
	AGrappleReactorTester();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Invokes the ApplyForce method on the grapple reactor
	void OnButtonOnePressed();
	void OnButtonTwoPressed();

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
 
