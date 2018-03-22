// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEditorTool.h"
#include "BalancedFPSLevelGeneratorTool.generated.h"

/**
 * This is the main class of this bundle, that handles the top-layer of level generation.
 * Functionality for certain components of this level generation, is handled by other
 * classes.
 */
UCLASS()
class BALANCEDFPSLEVELGENERATOR_API UBalancedFPSLevelGeneratorTool : public UBaseEditorTool
{
	GENERATED_BODY()
	
public:

	// Functions/Methods:

	/** Standard constructor. */
	UBalancedFPSLevelGeneratorTool();

	/** Execute the generation algorithms. */
	UFUNCTION(Exec)
	void GenerateLevel();

	// Properties:

	/** For testing how these are shown in the menu. */
	UPROPERTY(EditAnywhere, Category = "Core Properties")
	FVector2D LevelExtents;

	UPROPERTY(EditAnywhere, Category = "Core Properties")
	float VariationProbability;

	UPROPERTY(EditAnywhere, Category = "Core Properties")
	float RecombinationProbability;

	UPROPERTY(EditAnywhere, Category = "Core Properties")
	float MutationValue;

	UPROPERTY(EditAnywhere, Category = "Core Properties")
	float AddMutationProbability;

	UPROPERTY(EditAnywhere, Category = "Core Properties")
	float MultiplyMutationProbability;

	/** For where to start generating the level from. */
	UPROPERTY(EditDefaultsOnly, Category = "Core Properties")
	FVector LevelGenerationStartPoint;

private:

	// Functions/Methods:

	/** Create a box that encapsulates the area defined by the user. */
	void InitialiseLevelGenerationArea();

	/** First, static-mesh actors are used to create the box. */
	void EncapsulateLevelGenerationArea();

	/** Then spawn a light source, for that area. */
	void AddLightSourceToLevelGenerationArea();

	/** 
	* Now the generator will populate that area with 
	* Zones (Wang Tiles). 
	*/
	void AddZonesToLevelGenerationArea();

	/** For determining which tile to use. */
	class UBlueprint* GetSuitableZoneTile(FVector2D CurrentPlacementPosition, TArray<class AZone*> LevelZoneTiles);

	// Properties:

	/** The default scale for the panels of the level. */
	FVector DefaultRelativePanelScale;

	/**
	* The Blueprint (with a static-mesh component)
	* that will be displayed in the scene, for the panels
	* of the Level Generation Area.
	*/
	UBlueprint* WallPanelBlueprintAsset;

	/** All of the Zone Blueprints (Wang Tiles) to be used in level generation. */
	TArray<UBlueprint*> LevelZoneTileBlueprints;

	/** For all of the zones placed in the level (from the LevelZoneTileBlueprints set). */
	TArray<AZone*> PlacedLevelZones;

	// Constant Values:

	const int TOTAL_ZONE_BLUEPRINT_COUNT = 11;

	/** 
	* To refer to the dimensions of the panels/tiles.
	* Taken from the values noted in the static-mesh
	* -editor, for Shape_Cube.
	*/
	const float DEFAULT_TILE_WIDTH = 100.0f;
	const float DEFAULT_TILE_HEIGHT = 100.0f;
	const float DEFAULT_TILE_DEPTH = 10.0f;
};
