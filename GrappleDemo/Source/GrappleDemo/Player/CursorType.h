// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"
#include "CursorType.generated.h"

UENUM(BlueprintType)
enum ECursorType
{
	Normal,
	Highlighted,
	Assist,
	Ghost
};
