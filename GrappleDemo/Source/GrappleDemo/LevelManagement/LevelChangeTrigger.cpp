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

void ALevelChangeTrigger::ChangeLevel()
{
	// Navigate to the stage given in the inspector.
	UGameplayStatics::OpenLevel(this, FName(*stageName));
}
