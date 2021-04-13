// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SaveData/AudioPreferenceData.h"
#include "SaveData/AudioPreferenceSave.h"
#include "SaveData/LevelProgress.h"
#include "SaveData/LevelCompletionData.h"
#include "SaveData/LevelCompletionSave.h"
#include "SaveData/CollectionMissionData.h"
#include "SaveData/CollectionMissionSave.h"
#include "GameStage.h"
#include "GrappleGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAudioPreferencesChanged, const FAudioPreferenceData, NewPreferences);

UCLASS()
class GRAPPLEDEMO_API UGrappleGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
private:
	// These have to be UPROPS otherwise they get GC'd.
	UPROPERTY()
	UAudioPreferenceSave* audioPreferencesSave;
	UPROPERTY()
	TMap<TEnumAsByte<EGameStage>, FLevelProgress> completionData;

public:
	// Commits all save data to disk.
	UFUNCTION(BlueprintCallable)
	void Save();

	// Gets the audio preferences, loading or getting default if necessary.
	UFUNCTION(BlueprintCallable)
	FAudioPreferenceData GetAudioPreferences();
	// Sets the audio preferences, saving the preferences to disk.
	UFUNCTION(BlueprintCallable)
	void SetAudioPreferences(const FAudioPreferenceData preferences);
	// Resets audio preferences to the default.
	UFUNCTION(BlueprintCallable)
	void ClearAudioPreferences();
	// Called whenever the audio preferences change.
	UPROPERTY(BlueprintAssignable)
	FAudioPreferencesChanged OnPreferencesChanged;

	// Querys the level progress for a given level name.
	UFUNCTION(BlueprintCallable)
	FLevelProgress GetLevelProgress(const EGameStage level);
	// Sets the level progress for a given level name.
	UFUNCTION(BlueprintCallable)
	void SetLevelProgress(const FLevelProgress progress, const EGameStage level);
	// Resets level progress for a specific stage.
	UFUNCTION(BlueprintCallable)
	void ClearLevelProgress(const EGameStage level);

private:
	void LoadAudioPreferences();
	void LoadLevelProgress(const EGameStage level);
	void ResetData(FAudioPreferenceData& data);
	void ResetData(FLevelProgress& data, const EGameStage level);
	FString GameStageToString(const TEnumAsByte<EGameStage> gameStage);
};
