// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "vector" // For misc. collections.
#include "FPSLevelGeneratorEdge.h" // For this Zone's Edges.
#include "Zone.generated.h"

/**
 * This class represents the area of a level, that the space-filling algorithm
 * (Wang Tiles, as of 13/03/2018), will use to fix components of the level 
 * together.
 */
UCLASS()
class BALANCEDFPSLEVELGENERATOR_API AZone : public AActor
{
	GENERATED_BODY()

public:

	// Properties:

	// Constant values:

	/** These values are used to idenfiy each Zone. */
	const FName WANG_TILE_ONE = "WangTile1";
	const FName WANG_TILE_TWO = "WangTile2";
	const FName WANG_TILE_THREE = "WangTile3";
	const FName WANG_TILE_FOUR = "WangTile4";
	const FName WANG_TILE_FIVE = "WangTile5";
	const FName WANG_TILE_SIX = "WangTile6";
	const FName WANG_TILE_SEVEN = "WangTile7";
	const FName WANG_TILE_EIGHT = "WangTile8";
	const FName WANG_TILE_NINE = "WangTile9";
	const FName WANG_TILE_TEN = "WangTile10";
	const FName WANG_TILE_ELEVEN = "WangTile11";
	const FName WANG_TILE_TWELVE = "WangTile12";
	const FName WANG_TILE_THIRTEEN = "WangTile13";
	const FName WANG_TILE_FOURTEEN = "WangTile14";
	const FName WANG_TILE_FIFTHTEEN = "WangTile15";
	const FName WANG_TILE_SIXTEEN = "WangTile16";
	const FName WANG_TILE_SEVENTEEN = "WangTile17";
	const FName WANG_TILE_EIGHTTEEN = "WangTile18";
	const FName WANG_TILE_NINETEEN = "WangTile19";
	const FName WANG_TILE_TWENTY = "WangTile20";
	const FName WANG_TILE_TWENTY_ONE = "WangTile21";
	const FName WANG_TILE_TWENTY_TWO = "WangTile22";

	/** For the default ZoneEdge properties (during initialisation). */
	static const int DEFAULT_ZONE_EDGE_COUNT = 4;

	// Functions/Methods:

	/** Default constructor (required by UE4). */
	AZone();

	/** For proper initialisation. */
	void InitialiseZone();
	
	/** Determine these Coefficients, now the Zone will be placed. */
	void DetermineDefensivenessAndFlankingCoefficients(float SurroundingZones,
		float AdjacentZones);

	// Get functions:

	float GetDefensivenessCoefficient();
	float GetFlankingCoefficient();
	float GetDispersonCoefficient();

private:

	// Properties:

	/** To hold all the objects in the Zone. */
	TArray<UStaticMeshComponent*> ZoneObjects;

	// For the Coefficients used in determining Zone placement:
	float DefensivenessCoefficient;
	float FlankingCoefficient;
	float DispersionCoefficient;

	// Constant Values:

	/** For calculating the Defensiveness and Flanking Coefficients of this Zone. */
	const float HIGHEST_ZONE_VOLUME = 5.0f;

	const FVector DEFAULT_ZONE_EXTENTS = FVector(100.0f, 100.0f, 100.0f);

	// Functions/Methods:

	/** Determine the initial values of this zone. */
	void DetermineInitialZoneValues();

	/** 
	* Initialise the execution of the calculations to find the Defensiveness 
	* Coefficient.
	*/
	void InitialiseDefensivenessCoefficientCalculations(std::vector<float> TouchingEdgeCount,
		float ZoneObjectVolume, float AdjacentZones);

	/** 
	* Perform the bulk of the calculations for finding the PathDensity. 
	* To in turn, determine the Defensiveness Coefficient.
	*/
	void FindNonAbsolutePathDensity(float& PathDensity, bool IsFlipFlopRequired, 
		std::vector<float> TouchingEdgeCount, float AdjacentZones);

	/** 
	* After the non-absolute path-density value has been found, then there are just
	* 3 more lines to determine the Defensiveness Coefficient.
	*/
	void FindDefensivenessCoefficient(float ZoneObjectVolume, float& PathDensity);
};
