// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEditorTool.h"

// Bespoke header files:
#include "Zone.h"

#include "BalancedFPSLevelGeneratorTool.generated.h"

// Structures:

/** For the edge-colours of all of the ZoneTiles. */
struct ZoneTileEdgeColours
{
	// Properties:

	std::vector<std::vector<FPSLevelGeneratorEdge::EdgeColour>> AllEdgeColours;

	// Functions/Methods:

	/** Standard constructor. */
	ZoneTileEdgeColours::ZoneTileEdgeColours(int TileCount)
	{
		AllEdgeColours = std::vector<std::vector<FPSLevelGeneratorEdge::EdgeColour>>(TileCount);

		for (int ZoneTileEdgeColoursIterator = 0; ZoneTileEdgeColoursIterator < AllEdgeColours.size();
			ZoneTileEdgeColoursIterator++)
		{
			AllEdgeColours[ZoneTileEdgeColoursIterator] = std::vector<FPSLevelGeneratorEdge::EdgeColour>(size_t(AZone::DEFAULT_ZONE_EDGE_COUNT));
		}
	}

	/** Default constructor (required by VS). */
	ZoneTileEdgeColours::ZoneTileEdgeColours()
	{

	}
};

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

	/** Execute the generation algorithm. */
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
	class UBlueprint* GetSuitableZoneTile(FVector2D CurrentPlacementPosition);

	/** For getting an index of the Zone to use. */
	int GetZoneChoiceIndex();

	/** Match against Edges of a certain colour. */
	FPSLevelGeneratorEdge::EdgeColour MatchAgainstColourless(int RNGResult);
	FPSLevelGeneratorEdge::EdgeColour MatchAgainstGrey(int RNGResult);
	FPSLevelGeneratorEdge::EdgeColour MatchAgainstRed(int RNGResult);
	FPSLevelGeneratorEdge::EdgeColour MatchAgainstGreen(int RNGResult);
	FPSLevelGeneratorEdge::EdgeColour MatchAgainstBlue(int RNGResult);

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

	/** For the Zone Blueprints to spawn. */
	TArray<AZone*> LevelZones;

	/** For the subset of zones to pick from. */
	TArray<AZone*> ZoneSubSet;

	/** For the edge-colours of a Zone to match against. */
	std::vector<FPSLevelGeneratorEdge::EdgeColour> TargetEdgeColours;

	/** For all of the zones placed in the level (from the LevelZoneTileBlueprints set). */
	TArray<AZone*> PlacedLevelZones;

	/** For the edge-colours of all of the zones. */
	ZoneTileEdgeColours ZonesEdgeColours;

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

	/** 
	* The number of edges that have to match,
	* if there is no asbolute match.
	*/
	const int INCOMPLETE_MATCH_THRESHOLD = 2;

	// Used in comparison between a Zone's Edges: 
	
	/** Colourless against another. */
	const int COLOURLESS_TO_BLUE = 5;
	const int COLOURLESS_TO_GREEN = 10;
	const int COLOURLESS_TO_RED = 75;
	const int COLOURLESS_TO_GREY = 10;

	/** Blue against another. */
	const int BLUE_TO_BLUE = 50;
	const int BLUE_TO_GREEN = 0;
	const int BLUE_TO_RED = 0;
	const int BLUE_TO_GREY = 0;
	const int BLUE_TO_COLOURLESS = 50;

	/** Green against another. */
	const int GREEN_TO_BLUE = 0;
	const int GREEN_TO_GREEN = 5;
	const int GREEN_TO_RED = 75;
	const int GREEN_TO_GREY = 10;
	const int GREEN_TO_COLOURLESS = 10;

	/** Red against another. */
	const int RED_TO_BLUE = 0;
	const int RED_TO_GREEN = 5;
	const int RED_TO_RED = 75;
	const int RED_TO_GREY = 10;
	const int RED_TO_COLOURLESS = 10;

	/** Grey against another. */
	const int GREY_TO_BLUE = 0;
	const int GREY_TO_GREEN = 5;
	const int GREY_TO_RED = 85;
	const int GREY_TO_GREY = 5;
	const int GREY_TO_COLOURLESS = 5;
};
