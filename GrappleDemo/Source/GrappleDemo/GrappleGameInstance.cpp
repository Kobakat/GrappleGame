// Fill out your copyright notice in the Description page of Project Settings.


#include "GrappleGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UGrappleGameInstance::Save()
{
	// Save all data to disk.
	UGameplayStatics::SaveGameToSlot(audioPreferencesSave, "audioPreferences", 0);
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

void UGrappleGameInstance::ResetData(FAudioPreferenceData& data)
{
	data.Master = 60.F;
	data.Music = 100.F;
	data.Sound = 100.F;
}
