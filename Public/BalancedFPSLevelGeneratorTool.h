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

	/** 
	* UPROPERTY macro usage here allows these properties to be edited
	* in the details panel, that is shown when the user opens this tool,
	* via the edit sub-menu option.
	*/
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

	/** For the tag used to identify Blueprints already in the level, when it's loaded. */
	const FName TILE_SPAWN_BLUEPRINT_TAG = "TileSpawnBlueprint";
	/** For a count of all of the Blueprints that represent Zones. */
	const int TOTAL_ZONE_BLUEPRINT_COUNT = 22;

	// Other default tile properties:

	/** The default scale of Zones (can be adjusted for small */
	FVector DEFAULT_ZONE_SCALE = FVector(1.0f, 1.0f, 1.0f);

	/** To offset the placement of zones (to fall within the level generation area). */
	const FVector2D ZONE_POSITION_OFFSET = FVector2D(50.0f, 50.0f);

	/** 
	* To refer to the appropriate dimensions of the panels/tiles.
	* Taken from the values noted in the static-mesh
	* -editor, for Shape_Cube.
	*/
	const float DEFAULT_TILE_WIDTH = 100.0f;
	const float DEFAULT_TILE_HEIGHT = 100.0f;

	const float DEFAULT_TILE_Z_POSITION = 40.0f;

	/** Where one panel equates to 100x100x100, for the default level extents. */
	const FVector2D DEFAULT_LEVEL_EXTENTS = FVector2D(300.0f, 300.0f);
	
	/** 
	* For the offset of the encapsulation pieces,
	* from the level-generation area (arithmetic
	* operations are also used on this value).
	*/
	const float DEFAULT_ENCAPSULATION_OFFSET = 10.0f;

	// For choosing from a pre-defined set of zones:

	const int FIRST_OPTION = 1;
	const int SECOND_OPTION = 2;
	const int THIRD_OPTION = 3;

	// For the relative locations of the corners of a 
	// level-generation area:

	FVector2D TopLeftCorner;
	FVector2D TopRightCorner;
	FVector2D BottomRightCorner;
	FVector2D BottomLeftCorner;

	// For the indices of the Zones (stored in a TArray):

	const int ZONE_ONE_INDEX = 0;
	const int ZONE_TWO_INDEX = 1;
	const int ZONE_THREE_INDEX = 2;
	const int ZONE_FOUR_INDEX = 3;
	const int ZONE_FIVE_INDEX = 4;
	const int ZONE_SIX_INDEX = 5;
	const int ZONE_SEVEN_INDEX = 6;
	const int ZONE_EIGHT_INDEX = 7;
	const int ZONE_NINE_INDEX = 8;
	const int ZONE_TEN_INDEX = 9;
	const int ZONE_ELEVEN_INDEX = 10;
	const int ZONE_TWELVE_INDEX = 11;
	const int ZONE_THIRTEEN_INDEX = 12;
	const int ZONE_FOURTEEN_INDEX = 13;
	const int ZONE_FIFTHTEEN_INDEX = 14;
	const int ZONE_SIXTEEN_INDEX = 15;
	const int ZONE_SEVENTEEN_INDEX = 16;
	const int ZONE_EIGHTEEN_INDEX = 17;
	const int ZONE_NINETEEN_INDEX = 18;
	const int ZONE_TWENTY_INDEX = 19;
	const int ZONE_TWENTY_ONE_INDEX = 20;
	const int ZONE_TWENTY_TWO_INDEX = 21;
	
	/** 
	* The number of edges that have to match,
	* if there is no asbolute match.
	*/
	const int INCOMPLETE_MATCH_THRESHOLD = 2;

	// Used in comparison between a Zone's Edges: 
	
	/** Colourless against another. */
	const int COLOURLESS_TO_BLUE = 50;
	const int COLOURLESS_TO_GREEN = 30;
	const int COLOURLESS_TO_RED = 20;
	const int COLOURLESS_TO_GREY = 0;

	/** Blue against another. */
	const int BLUE_TO_BLUE = 20;
	const int BLUE_TO_GREEN = 30;
	const int BLUE_TO_RED = 50;
	const int BLUE_TO_GREY = 0;

	/** Green against another. */
	const int GREEN_TO_BLUE = 25;
	const int GREEN_TO_GREEN = 35;
	const int GREEN_TO_RED = 40;
	const int GREEN_TO_GREY = 0;

	/** Red against another. */
	const int RED_TO_BLUE = 50;
	const int RED_TO_GREEN = 30;
	const int RED_TO_RED = 20;
	const int RED_TO_GREY = 0;

	/** Grey against another. */
	const int GREY_TO_BLUE = 20;
	const int GREY_TO_GREEN = 30;
	const int GREY_TO_RED = 50;
	const int GREY_TO_GREY = 0;
};
// Previous values:

/** Colourless against another. */
//const int COLOURLESS_TO_BLUE = 5;
//const int COLOURLESS_TO_GREEN = 10;
//const int COLOURLESS_TO_RED = 75;
//const int COLOURLESS_TO_GREY = 10;

///** Blue against another. */
//const int BLUE_TO_BLUE = 85;
//const int BLUE_TO_GREEN = 5;
//const int BLUE_TO_RED = 5;
//const int BLUE_TO_GREY = 5;

///** Green against another. */
//const int GREEN_TO_BLUE = 5;
//const int GREEN_TO_GREEN = 10;
//const int GREEN_TO_RED = 75;
//const int GREEN_TO_GREY = 10;

///** Red against another. */
//const int RED_TO_BLUE = 5;
//const int RED_TO_GREEN = 10;
//const int RED_TO_RED = 75;
//const int RED_TO_GREY = 10;

///** Grey against another. */
//const int GREY_TO_BLUE = 5;
//const int GREY_TO_GREEN = 5;
//const int GREY_TO_RED = 85;
//const int GREY_TO_GREY = 5;