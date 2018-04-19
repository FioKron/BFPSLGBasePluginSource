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

	/** Execute the generation algorithm. */
	UFUNCTION(Exec)
	void GenerateLevel();

	// Properties:

	// Enumerations:

	enum ZoneAdjacencyDirection
	{
		Westwards,
		Southwards,
		Eastwords,
		Northwords
	};

	/** 
	* UPROPERTY macro usage here allows these properties to be edited
	* in the details panel, that is shown when the user opens this tool,
	* via the edit sub-menu option.
	*/
	UPROPERTY(EditAnywhere, Category = "Core Properties")
	FVector2D LevelExtents;

	/** For where to start generating the level from. */
	UPROPERTY(EditDefaultsOnly, Category = "Core Properties")
	FVector LevelGenerationStartPoint;

private:

	// Functions/Methods:

	/** Create a box that encapsulates the area defined by the user. */
	void InitialiseLevelGenerationArea();

	/** First, static-mesh actors are used to create the box. */
	void EncapsulateLevelGenerationArea();

	void SpawnTopOrBottomWallPanelTileForCurrentPosition(FVector PanelTilePosition, bool IsTopFaceTile);

	/** Then spawn a light source, for that area. */
	void AddLightSourceToLevelGenerationArea();

	/** 
	* Now the generator will populate that area with 
	* Zones (Wang Tiles). 
	*/
	void AddZonesToLevelGenerationArea();

	/** For determining which tile to use. */
	class UBlueprint* GetSuitableZoneTile(FVector2D CurrentPlacementPosition);

	/** 
	* This function also retrives a Zone index,
	* but uses the Coefficients for comparison, intead
	* of the Edge-colours.
	*/
	int GetZoneConsideringCoefficients(int ZoneToCompareTo, ZoneAdjacencyDirection PlacedZoneAdjacency);

	/** Get the Zone corresponding to the ZoneChoice. */
	UBlueprint* GetTargetZone(int ZoneChoice);

	// Helper functions:

	bool ZoneIsEdgePiece(int ConsideredZone);
	bool ZoneIsCornerPiece(int ConsideredZone);
	bool ZoneHasPureEvenZoneDispersion(float ConsideredZoneDispersionCoefficient);
	bool ZoneHasHalfEvenZoneDispersion(float ConsideredZoneDispersionCoefficient);
	bool ZoneIsWangTile2(int ConsideredZone);
	bool ZoneIsWangTile10(int ConsideredZone);
	void FindApplicableZoneIndices(std::vector<int>& ApplicableZoneIndices);
	int GetApplicableZoneIndex(std::vector<int>& ApplicableZoneIndices);
	int PickZoneConsideringAdjacencyToWangTile10Or2(
		ZoneAdjacencyDirection PlacedZoneAdjacency, int ConsideredAdjacentZoneID);
	
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

	/** For all of the zones placed in the level (from the LevelZoneTileBlueprints set). */
	TArray<AZone*> PlacedLevelZones;

	/** 
	* For the relative locations of the corners of a 
	* level-generation area:
	*/
	FVector2D TopLeftCorner;
	FVector2D TopRightCorner;
	FVector2D BottomRightCorner;
	FVector2D BottomLeftCorner;

	/** 
	* For determining which Zone to choose from, based on Coefficient 
	* comparison between a given Zone and either WangTile2 or WangTile10.
	*/
	std::vector<int> ApplicableZoneIndicesForWangTile2;
	std::vector<int> ApplicableZoneIndicesForWangTile10;

	// Constant Values:

	/** 
	* For the tag used to identify Blueprints already in the level, when it's loaded
	* or a level has already been generated. 
	*/
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

	/** 
	* This value is for which XY-plane the Wang Tiles of the level should
	* be placed.
	*/
	const float DEFAULT_TILE_Z_POSITION = 40.0f;

	/** Where one panel equates to 100x100x100, for the default level extents. */
	const FVector2D DEFAULT_LEVEL_EXTENTS = FVector2D(300.0f, 300.0f);
	
	/** 
	* For the offset of the encapsulation pieces,
	* from the level-generation area (arithmetic
	* operations are also used on this value).
	*/
	const float DEFAULT_ENCAPSULATION_OFFSET = 10.0f;

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

	// Used in comparsion between coefficients:

	/** Only 1 Component in the Zone. */
	const float PURE_EVEN_ZONE_DISPERSION = 1.0f;

	/** For 2 Components in the zone. */
	const float HALF_EVEN_ZONE_DISPERSION = 0.50f;

	/** For a corner piece. */
	const float CORNER_PIECE_ZONE_DISPERSION = 0.250f;
};
