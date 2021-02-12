// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleReactorTester.h"
#include "Components/InputComponent.h"

// Sets default values
AGrappleReactorTester::AGrappleReactorTester()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Automatically posseses this pawn.
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AGrappleReactorTester::BeginPlay()
{
	Super::BeginPlay();
}

// Called to bind functionality to input
void AGrappleReactorTester::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// These input actions should not be on main
	InputComponent->BindAction("Jethro_Test_Mapping_1", InputMode, this, &AGrappleReactorTester::OnButtonOnePressed);
	InputComponent->BindAction("Jethro_Test_Mapping_2", InputMode, this, &AGrappleReactorTester::OnButtonTwoPressed);
}
// Run the simulated pull forces
void AGrappleReactorTester::OnButtonOnePressed()
{
	Reactor->ApplyPullForce(KeyNineForceVector);
}
void AGrappleReactorTester::OnButtonTwoPressed()
{
	Reactor->ApplyPullForce(KeyZeroForceVector);
}

