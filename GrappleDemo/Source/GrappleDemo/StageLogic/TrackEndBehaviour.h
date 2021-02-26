// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/ObjectMacros.h"
#include "TrackEndBehaviour.generated.h"

UENUM()
enum ETrackEndBehaviour
{
	LoopAround,
	PingPong,
	StopAtEnds
};
