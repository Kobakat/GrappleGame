// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleReactor.h"
#include "CollisionQueryParams.h"

// Sets default values
AGrappleReactor::AGrappleReactor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGrappleReactor::BeginPlay()
{
	Super::BeginPlay();
}

void AGrappleReactor::ForceDetach()
{
	// TODO add grapple null check,
	// call grapple detach
}

// Called every frame
void AGrappleReactor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrappleReactor::Hook()
{
	isHookedOnto = true;
	ReceiveOnHooked();
}

void AGrappleReactor::Unhook()
{
	isHookedOnto = false;
	ReceiveOnUnhooked();
}

void AGrappleReactor::ApplyPullForce(const FVector force)
{
	ReceiveOnForceApplied(force);
}

