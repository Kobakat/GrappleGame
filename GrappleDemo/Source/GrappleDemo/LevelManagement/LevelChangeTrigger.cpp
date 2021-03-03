// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelChangeTrigger.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// Sets default values
ALevelChangeTrigger::ALevelChangeTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelChangeTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelChangeTrigger::ChangeLevel()
{
	UWorld* world = level.Get();
	// Make sure the inspector reference was set.
	if (world != nullptr)
		UGameplayStatics::OpenLevel(this, FName(*world->GetMapName()));
}


