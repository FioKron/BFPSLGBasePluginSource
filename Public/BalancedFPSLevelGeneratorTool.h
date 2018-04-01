// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEditorTool.h"

// Bespoke header files:
#include "Zone.h"

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

	/** For getting an index of the Zone to use. */
	int GetZoneChoiceIndex(TArray<AZone*>& ZoneSubsetReference,
		std::vector<FPSLevelGeneratorEdge::EdgeColour>& TargetEdgeColoursReference);

	/** To remove zones that would not match up to the select edge-comparison. */
	void UpdateValidZonesForPlacement(TArray<AZone*>& ZoneSubsetReference,
		FPSLevelGeneratorEdge::EdgeColour TargetEdgeColour,
		int CurrentEdgePosition);

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

	// Used in comparison between a Zone's Edges: 
	
	/** Colourless against another. */
	const int COLOURLESS_TO_BLUE = 5;
	const int COLOURLESS_TO_GREEN = 5;
	const int COLOURLESS_TO_RED = 5;
	const int COLOURLESS_TO_GREY = 20;
	const int COLOURLESS_TO_COLOURLESS = 65;

	/** Blue against another. */
	const int BLUE_TO_BLUE = 84;
	const int BLUE_TO_GREEN = 4;
	const int BLUE_TO_RED = 4;
	const int BLUE_TO_GREY = 4;
	const int BLUE_TO_COLOURLESS = 4;

	/** Green against another. */
	const int GREEN_TO_BLUE = 5;
	const int GREEN_TO_GREEN = 10;
	const int GREEN_TO_RED = 15;
	const int GREEN_TO_GREY = 20;
	const int GREEN_TO_COLOURLESS = 55;

	/** Red against another. */
	const int RED_TO_BLUE = 5;
	const int RED_TO_GREEN = 15;
	const int RED_TO_RED = 20;
	const int RED_TO_GREY = 25;
	const int RED_TO_COLOURLESS = 35;

	/** Grey against another. */
	const int GREY_TO_BLUE = 4;
	const int GREY_TO_GREEN = 4;
	const int GREY_TO_RED = 4;
	const int GREY_TO_GREY = 4;
	const int GREY_TO_COLOURLESS = 84;
};
