// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BalancedFPSLevelGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FBalancedFPSLevelGeneratorModule"

void FBalancedFPSLevelGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Generate Level (BalancedFPSLevelGenerator)",
		"Generate a level using the BalancedFPSLevelGenerator",
		EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
