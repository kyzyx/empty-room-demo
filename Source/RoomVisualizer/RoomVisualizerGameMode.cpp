// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "RoomVisualizer.h"
#include "RoomVisualizerGameMode.h"
#include "RoomVisualizerHUD.h"
#include "RoomVisualizerCharacter.h"

ARoomVisualizerGameMode::ARoomVisualizerGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARoomVisualizerHUD::StaticClass();
}
