// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEditorTool.h"

// Bespoke header files:
#include "Zone.h"

// For storing misc. data:
#include <map>
#include <utility>

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

	/** To check where Zone(s) are relative to the current position. */
	enum ZoneAdjacencyDirection
	{
		Westwards,
		Southwards,
		Eastwords,
		Northwords
	};

	/** For which collection to choose a Zone index from. */
	enum ZoneCollectionToChoose
	{
		WangTile2Collection,
		Wangtile10Collection,
		OtherCollection
	};

	// Structures:

	/** 
	* Used in checking if there are Zones to the south and west,
	* along with what the indices of these Zones are, if this is the
	* case.
	*/
	struct ZoneToSouthAndOrWestCheck
	{
	public:

		// Properties:

		// Core struct properties:

		bool IsZoneToSouth;
		bool IsZoneToWest;
		int ZoneToSouthAndWestIndices[2];

		// Functions/Methods:

		/** Standard constructor. */
		ZoneToSouthAndOrWestCheck()
		{
			IsZoneToSouth = false;
			IsZoneToWest = false;
			for (int CurrentIndex : ZoneToSouthAndWestIndices)
			{
				CurrentIndex = -1;
			}
		}
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
	* but uses the Coefficients for comparison, instead
	* of the Edge-colours.
	*/
	int GetZoneConsideringCoefficients(int ZoneToCompareTo);

	/** For placement of a Zone, that will have 4 Zones adjacent to it. */
	int GetZoneConsideringCoefficientsForZonesSouthAndWestOfPosition(int ZonesToCompareTo[2]);

	/** Get the Zone corresponding to the ZoneChoice. */
	UBlueprint* GetTargetZone(int ZoneChoice);

	// Helper functions:

	// Checking what piece or particular Wang-Tile this Zone is:

	bool ZoneIsEdgePiece(int ConsideredZone);
	bool ZoneIsCornerPiece(int ConsideredZone);
	bool ZoneIsWangTile2Or10(int ConsideredZone, bool CheckingForWangTile2);
	
	// For getting indices:
	void FindApplicableZoneIndicesConsideringDispersion(float DispersionCoefficient);
	int GetApplicableZoneIndex(ZoneCollectionToChoose CollectionToConsider);
	int PickZoneConsideringAdjacencyToWangTile10Or2(int ConsideredAdjacentZoneID);
	void FindApplicableZoneIndicesConsideringDefensiveness(bool IsGreaterThanThreshold);

	// Conditional checks:

	// Considering Dispersion:

	bool ZoneHasPureEvenZoneDispersion(float ConsideredZoneDispersionCoefficient);
	bool ZoneHasHalfEvenZoneDispersion(float ConsideredZoneDispersionCoefficient);

	// Other conditions:

	bool PlacedZoneDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(
		int ZoneIndexToCheckAgainstThreshold, bool IsGreaterThanOrEqualToCheck);
	bool ZoneSubsetDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(
		int ZoneIndexToCheckAgainstThreshold, bool IsGreaterThanOrEqualToCheck);
	ZoneToSouthAndOrWestCheck ZoneToSouthAndOrWest(FVector2D CurrentPlacementPosition);

	// To streamline adding to collections:
	void AddValueToCurrentPlacedZonePositions(FVector2D PositionValue);

	// Properties:

	/** 
	* For populating with applicable Zone indices, after determining such,
	* based on the Coefficients of Zones.
	*/
	std::vector<int> ApplicableZoneIndices;

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

	/** For all of the zones placed in the level (from the LevelZoneTileBlueprints set). */
	TArray<AZone*> PlacedLevelZones;

	/** As for some reason, the position of the Zones would not match-up to their actual position. */
	std::map<int, FVector2D> PlacedZonePositions;
	/** For the key's value, of PlacedZonePositions. */
	int CurrentPlacedZonePositionsKeyValue;

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
	* be placed upon.
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

	// Used in comparsion between Coefficients:

	/** Only 1 Component in the Zone. */
	const float PURE_EVEN_ZONE_DISPERSION = 1.0f;

	/** For 2 Components in the zone. */
	const float HALF_EVEN_ZONE_DISPERSION = 0.50f;

	/** For a corner piece. */
	const float CORNER_PIECE_ZONE_DISPERSION = 0.250f;

	/** For comparing Zone Defensiveness Coefficient Values. */
	const float ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD = 0.80f;
};
