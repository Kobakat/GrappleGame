// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelChangeTrigger.generated.h"

UCLASS()
class GRAPPLEDEMO_API ALevelChangeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChangeTrigger();

protected:
	UPROPERTY(EditAnywhere, Category = "Target Map")
	TSoftObjectPtr<UWorld> level;

public:
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Change Level"))
	void ChangeLevel();

};
