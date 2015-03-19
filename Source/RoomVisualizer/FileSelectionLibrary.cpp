// Fill out your copyright notice in the Description page of Project Settings.

#include "RoomVisualizer.h"
#include "FileSelectionLibrary.h"
#include "Developer/DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Engine.h"
#include "Paths.h"

FString UFileSelectionLibrary::GetSaveFilename() {
	TArray<FString> files;
	FDesktopPlatformModule::Get()->SaveFileDialog(
		GetDesktopWindow(),
		TEXT("Save File As..."),
		FPaths::GameSavedDir(),
		TEXT(""),
		TEXT("JSON file|*.json"),
		0,
		files);
	if (files.Num() == 0)
		return "";
	else
		return files[0];
}


FString UFileSelectionLibrary::GetOpenFilename() {
	TArray<FString> files;
	FDesktopPlatformModule::Get()->OpenFileDialog(
		GetDesktopWindow(),
		"Open File...",
		FPaths::GameSavedDir(),
		"",
		"JSON file|*.json",
		0,
		files);
	if (files.Num() == 0)
		return "";
	else
		return files[0];
}
