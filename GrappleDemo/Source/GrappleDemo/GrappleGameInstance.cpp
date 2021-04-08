// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UGrappleGameInstance::Save()
{
	// Save all data to disk.
	UGameplayStatics::SaveGameToSlot(audioPreferencesSave, "audioPreferences", 0);
	// Save each stage to disk.
	for (const TPair<TEnumAsByte<EGameStage>, FLevelProgress>& pair : completionData)
	{
		FString slotName = GameStageToString(pair.Key);
		// Create the save object for base level data.
		ULevelCompletionSave* levelCompletionSave = Cast<ULevelCompletionSave>(
			UGameplayStatics::CreateSaveGameObject(
				ULevelCompletionSave::StaticClass()));
		levelCompletionSave->CompletionData = pair.Value.CompletionData;
		UGameplayStatics::SaveGameToSlot(levelCompletionSave, slotName, 0);
		// Iterate through creating the save data for each objective.
		for (int i = 0; i < pair.Value.CollectionMissions.Num(); i++)
		{
			UCollectionMissionSave* missionSave = Cast<UCollectionMissionSave>(
				UGameplayStatics::CreateSaveGameObject(
					UCollectionMissionSave::StaticClass()));
			missionSave->CollectionMissionData = pair.Value.CollectionMissions[i];
			FString collectionSlotName =
				slotName + "collection" + FString::FromInt(i);
			UGameplayStatics::SaveGameToSlot(missionSave, collectionSlotName, 0);
		}
	}
}

FAudioPreferenceData UGrappleGameInstance::GetAudioPreferences()
{
	// Ensure the save file is checked out.
	if (audioPreferencesSave == nullptr)
		LoadAudioPreferences();
	// Return the preference data.
	return audioPreferencesSave->AudioPreferences;
}

void UGrappleGameInstance::SetAudioPreferences(const FAudioPreferenceData preferences)
{
	// Ensure the save file is checked out.
	if (audioPreferencesSave == nullptr)
		LoadAudioPreferences();
	// Update the data.
	audioPreferencesSave->AudioPreferences = preferences;
	// Notify listeners.
	OnPreferencesChanged.Broadcast(preferences);
}

void UGrappleGameInstance::ClearAudioPreferences()
{
	// Ensure the save file is checked out.
	if (audioPreferencesSave == nullptr)
		LoadAudioPreferences();
	// Set default save values.
	ResetData(audioPreferencesSave->AudioPreferences);
	// Notify listeners.
	OnPreferencesChanged.Broadcast(audioPreferencesSave->AudioPreferences);
}

FLevelProgress UGrappleGameInstance::GetLevelProgress(const EGameStage level)
{
	// Has this data already been loaded?
	if (!completionData.Contains(level))
		LoadLevelProgress(level);
	// Return the stage data.
	return completionData[level];
}

void UGrappleGameInstance::SetLevelProgress(const FLevelProgress progress, const EGameStage levelName)
{
	// Update the saved progress.
	if (completionData.Contains(levelName))
		completionData[levelName] = progress;
	else
		completionData.Add(levelName, progress);
}

void UGrappleGameInstance::ClearLevelProgress(const EGameStage level)
{
	// Reset level to default progress.
	FLevelProgress resetProgress;
	ResetData(resetProgress, level);
	SetLevelProgress(resetProgress, level);
}

void UGrappleGameInstance::LoadAudioPreferences()
{
	// Does the save exist?
	if (UGameplayStatics::DoesSaveGameExist("audioPreferences", 0))
	{
		// If it does then load it.
		audioPreferencesSave = Cast<UAudioPreferenceSave>(
			UGameplayStatics::LoadGameFromSlot("audioPreferences", 0));
	}
	else
	{
		// Create the save object.
		audioPreferencesSave = Cast<UAudioPreferenceSave>(
			UGameplayStatics::CreateSaveGameObject(
				UAudioPreferenceSave::StaticClass()));
		// Set default save values.
		ResetData(audioPreferencesSave->AudioPreferences);
	}
}

void UGrappleGameInstance::LoadLevelProgress(const EGameStage level)
{
	// Get the default values to compare save changes.
	FLevelProgress defaultData;
	ResetData(defaultData, level);
	// Does the save exist?
	FString slotName = UEnum::GetValueAsString<EGameStage>(level);
	if (UGameplayStatics::DoesSaveGameExist(slotName, 0))
	{
		// If it does then load it.
		ULevelCompletionSave* levelSave = Cast<ULevelCompletionSave>(
			UGameplayStatics::LoadGameFromSlot(slotName, 0));
		// Check for mismatches along each mission type.
		// Accumulate all saved collection missions.
		TArray<UCollectionMissionSave*> collectionMissions;
		int i = 0;
		while (true)
		{
			FString collectionSlotName =
				slotName + "collection" + FString::FromInt(i);
			if (UGameplayStatics::DoesSaveGameExist(collectionSlotName, 0))
				collectionMissions.Add(
					Cast<UCollectionMissionSave>(
						UGameplayStatics::LoadGameFromSlot(collectionSlotName, 0)));
			else
				break;
			i++;
		}
		// Truncate off save data that might have been removed in a patch.
		if (collectionMissions.Num() > defaultData.CollectionMissions.Num())
			collectionMissions.SetNum(defaultData.CollectionMissions.Num());
		// If collectable counts have changed, reset the mission.
		for (int j = 0; j < collectionMissions.Num(); j++)
		{
			if (collectionMissions[j]->CollectionMissionData.CollectedStates.Num()
				!= defaultData.CollectionMissions[j].CollectedStates.Num())
			{
				collectionMissions[j]->CollectionMissionData.CollectedStates.SetNum(0);
				collectionMissions[j]->CollectionMissionData.CollectedStates.SetNumZeroed(
					defaultData.CollectionMissions[j].CollectedStates.Num());
			}
		}
		// Load all of the data into the object to return.
		FLevelProgress loadedData;
		loadedData.CompletionData = levelSave->CompletionData;
		for (int k = 0; k < collectionMissions.Num(); k++)
			loadedData.CollectionMissions.Add(collectionMissions[k]->CollectionMissionData);
		completionData.Add(level, loadedData);
	}
	else
	{
		// Set default save values.
		completionData.Add(level, defaultData);
	}
}

void UGrappleGameInstance::ResetData(FAudioPreferenceData& data)
{
	data.Master = 60.F;
	data.Music = 100.F;
	data.Sound = 100.F;
}

void UGrappleGameInstance::ResetData(FLevelProgress& data, const EGameStage level)
{
	data.CompletionData.StageCompleted = false;
	data.CompletionData.BestTime = FLT_MAX;
	// Set mission data based on stage.
	switch (level)
	{
	case EGameStage::Tutorial:
		data.CollectionMissions.SetNum(0);
		break;
	case EGameStage::WalkingTutorial:
		data.CollectionMissions.SetNum(0);
		break;
	case EGameStage::PyramidTower:
		data.CollectionMissions.SetNum(1);
		data.CollectionMissions[0].AllAreCollected = false;
		data.CollectionMissions[0].CollectedStates.SetNum(0);
		data.CollectionMissions[0].CollectedStates.SetNumZeroed(5);
		break;
	case EGameStage::SearsTower:
		data.CollectionMissions.SetNum(1);
		data.CollectionMissions[0].AllAreCollected = false;
		data.CollectionMissions[0].CollectedStates.SetNum(0);
		data.CollectionMissions[0].CollectedStates.SetNumZeroed(5);
		break;
	}
}

FString UGrappleGameInstance::GameStageToString(const TEnumAsByte<EGameStage> gameStage)
{
	switch (gameStage)
	{
	case EGameStage::Tutorial:
		return UEnum::GetValueAsString<EGameStage>(EGameStage::Tutorial);
	case EGameStage::WalkingTutorial:
		return UEnum::GetValueAsString<EGameStage>(EGameStage::WalkingTutorial);
	case EGameStage::PyramidTower:
		return UEnum::GetValueAsString<EGameStage>(EGameStage::PyramidTower);
	case EGameStage::SearsTower:
		return UEnum::GetValueAsString<EGameStage>(EGameStage::SearsTower);
	}
	return "";
}
