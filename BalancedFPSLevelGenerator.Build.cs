// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BalancedFPSLevelGenerator : ModuleRules
{
	public BalancedFPSLevelGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"BalancedFPSLevelGenerator/Public"
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"BalancedFPSLevelGenerator/Private",
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "CoreUObject",
                "Engine",
                "Slate",
                "AssetTools",
                "UnrealEd",                
            }
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Projects",
				"InputCore",
                "SlateCore",
                "LevelEditor",
                "PropertyEditor",
                "KismetWidgets",
                "KismetCompiler",
                "BlueprintGraph",
                "GraphEditor",
                "Kismet",
                "EditorStyle",
                "Slate",
                "Sequencer",
                "DetailCustomizations",
                "Settings",
                "RenderCore",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
