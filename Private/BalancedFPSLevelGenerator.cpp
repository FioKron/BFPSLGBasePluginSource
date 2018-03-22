// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BalancedFPSLevelGenerator.h"
#include "BalancedFPSLevelGeneratorStyle.h"
#include "BalancedFPSLevelGeneratorCommands.h"
#include "Misc/MessageDialog.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "PropertyEditorModule.h"
#include "BaseEditorToolCustomisation.h"
#include "BaseEditorTool.h"
#include "BalancedFPSLevelGeneratorTool.h"
#include "LevelEditor.h"
#include "UObjectIterator.h"

static const FName BalancedFPSLevelGeneratorTabName("BalancedFPSLevelGenerator");

#define LOCTEXT_NAMESPACE "FBalancedFPSLevelGeneratorModule"

void FBalancedFPSLevelGeneratorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FBalancedFPSLevelGeneratorStyle::Initialize();
	FBalancedFPSLevelGeneratorStyle::ReloadTextures();

	FBalancedFPSLevelGeneratorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);
		
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	
	// Register the detail customisation:
	{
		// Use "PropertyEditor" and not "PropertyEditorModule" for the parameter:
		FPropertyEditorModule& PropertyEditorModuleReference = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");		
		
		PropertyEditorModuleReference.RegisterCustomClassLayout("BaseEditorTool", FOnGetDetailCustomizationInstance::CreateStatic(
			&FBaseEditorToolCustomisation::MakeInstance));
		
		PropertyEditorModuleReference.NotifyCustomizationModuleChanged();
	}
	
	// For extending menus or the toolbar:
	struct Local
	{
		static void AddMenuCommands(FMenuBuilder& MenuBuilder)
		{
			MenuBuilder.AddSubMenu(LOCTEXT("Category", "Procedural"),
				LOCTEXT("CategoryTooltip", "Procedural Bundle"),
				FNewMenuDelegate::CreateStatic(&FBalancedFPSLevelGeneratorModule::CreateToolListMenu));
		}
	};

	{
		TSharedPtr<FExtender> MenuExtender(new FExtender());
		MenuExtender->AddMenuExtension(
			"EditMain", EExtensionHook::After,
			PluginCommands.ToSharedRef(),
			FMenuExtensionDelegate::CreateStatic(&Local::AddMenuCommands));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
}

void FBalancedFPSLevelGeneratorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FBalancedFPSLevelGeneratorCommands::Unregister();
	FBalancedFPSLevelGeneratorStyle::Shutdown();
}

void FBalancedFPSLevelGeneratorModule::TriggerTool(UClass* ToolClass)
{
	UBaseEditorTool* ToolInstance = NewObject<UBaseEditorTool>(GetTransientPackage(), ToolClass);
	ToolInstance->AddToRoot();

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<
		FPropertyEditorModule>("PropertyEditor");

	TArray<UObject*> ObjectsToView;
	ObjectsToView.Add(ToolInstance);
	TSharedRef<SWindow> PropertyWindow = PropertyModule.CreateFloatingDetailsView(ObjectsToView,
		/*bIsLockable=*/ false);
	PropertyWindow->SetOnWindowClosed(FOnWindowClosed::CreateStatic(&FBalancedFPSLevelGeneratorModule::
		OnToolWindowClosed, ToolInstance));
}

/** 
	Remove the tool-instance created in TriggerTool, 
	from the root set.
*/
void FBalancedFPSLevelGeneratorModule::OnToolWindowClosed(const TSharedRef<SWindow>& 
	WindowReference, UBaseEditorTool* Instance)
{
	Instance->RemoveFromRoot();
}

void FBalancedFPSLevelGeneratorModule::CreateToolListMenu(FMenuBuilder& MenuBuilderReference)
{
	for (TObjectIterator<UClass> ClassIterator; ClassIterator; ++ClassIterator)
	{
		UClass* CurrentClass = *ClassIterator;
		if (!CurrentClass->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists |
			CLASS_Abstract))
		{
			if (CurrentClass->IsChildOf(UBaseEditorTool::StaticClass()))
			{
				FString FriendlyName = CurrentClass->GetName();
				FText MenuDescription = FText::Format(LOCTEXT("ToolMenuDescription", "{0}"),
					FText::FromString(FriendlyName));
				FText MenuTooltip = FText::Format(LOCTEXT("ToolMenuTooltip", "Execute the {0} tool."),
					FText::FromString(FriendlyName));

				FUIAction ClassAction(FExecuteAction::CreateStatic(&FBalancedFPSLevelGeneratorModule::
					TriggerTool, CurrentClass));

				MenuBuilderReference.AddMenuEntry(
					MenuDescription,
					MenuTooltip,
					FSlateIcon(),
					ClassAction);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBalancedFPSLevelGeneratorModule, BalancedFPSLevelGenerator)