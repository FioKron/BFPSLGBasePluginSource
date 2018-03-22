// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FToolBarBuilder;
class FMenuBuilder;
class UBaseEditorTool;

class FBalancedFPSLevelGeneratorModule : public IModuleInterface
{
public:

	// Properties:

	// Functions/Methods:

	/** IModuleInterface implementation. */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static void TriggerTool(UClass* ToolClass);
	static void CreateToolListMenu(class FMenuBuilder& MenuBuilderReference);
	static void OnToolWindowClosed(const TSharedRef<SWindow>& WindowReference,
		UBaseEditorTool* Instance);

private:


	// Properties:

	TSharedPtr<class FUICommandList> PluginCommands;
};



