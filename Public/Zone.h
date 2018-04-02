// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "vector" // For misc. collections.
#include "FPSLevelGeneratorEdge.h" // For this Zone's Edges.
#include "Zone.generated.h"

/** As UFPSLevelGeneratorEdge properties were getting cleaned up. */
USTRUCT()
struct FPSLevelGeneratorEdge
{
	GENERATED_BODY()

public:

	// Operators:

	/** To remove complaints of the '=' operator being delated. */
	FPSLevelGeneratorEdge& FPSLevelGeneratorEdge::operator=(const FPSLevelGeneratorEdge& RightHandSide)
	{

	}

	// Enumerations:

	enum EdgeColour
	{
		Colourless,
		Grey,
		Red,
		Green,
		Blue
	};

	// Functions/Methods:

	/** Default constructor (required by UE4). */
	FPSLevelGeneratorEdge::FPSLevelGeneratorEdge()
	{

	}

	// Initialisation methods:

	void InitialiseEdge(FVector2D NewEdgeCentrePoint)
	{
		EdgeCentrePoint = NewEdgeCentrePoint;
	}

	/** EdgeColour is calculated manually for now, the result is parsed into this function. */
	void InitialiseEdgeColour(EdgeColour NewEdgeColour)
	{
		ThisEdgeColour = NewEdgeColour;
	}

	// Get functions:
	const EdgeColour GetEdgeColour()
	{
		return ThisEdgeColour;
	}

	// Properties:

	// Constant Values:

	/** For the relative centre-points of a respective edge. */
	const FVector2D DEFAULT_NORTH_EDGE_RELATIVE_CENTRE_POINT = FVector2D(0.0f, -45.0f);
	const FVector2D DEFAULT_EAST_EDGE_RELATIVE_CENTRE_POINT = FVector2D(45.0f, 0.0f);
	const FVector2D DEFAULT_SOUTH_EDGE_RELATIVE_CENTRE_POINT = FVector2D(0.0f, 45.0f);
	const FVector2D DEFAULT_WEST_EDGE_RELATIVE_CENTRE_POINT = FVector2D(-45.0f, 0.0f);

private:

	// Properties:

	/** For the colour of this edge. */
	EdgeColour ThisEdgeColour;

	/**
	* The centre point of this edge, used
	* in the calculations for this Edge's density.
	*/
	FVector2D EdgeCentrePoint;
};

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

	/** 
	* These represent the Edges of this zone (to be put into the ZoneEdges Array).
	* The RF_RootSet flag is used to make sure they are not cleaned up by the 
	* Garbage Collector.
	*/
	FPSLevelGeneratorEdge NorthEdge;
	FPSLevelGeneratorEdge EastEdge;
	FPSLevelGeneratorEdge SouthEdge;
	FPSLevelGeneratorEdge WestEdge;

	// Constant values:

	/** For the default ZoneEdge properties (during initialisation). */
	static const int DEFAULT_ZONE_EDGE_COUNT = 4;

	// Functions/Methods:

	/** Default constructor (required by UE4). */
	AZone();

	/** For proper initialisation. */
	void InitialiseZone();

	// Set methods:

	void SetZoneValidForPlacement(bool NewZoneValidForPlacement);
	
	// Get functions:

	/** 
	* Zone Edge order:
	* First element: North Edge.
	* Second element: East Edge.
	* Third element: South Edge.
	* Fourth element: West Edge.
	*/
	std::vector<FPSLevelGeneratorEdge> GetZoneEdges();

	std::vector<FPSLevelGeneratorEdge::EdgeColour> GetZoneEdgeColours();

	bool GetZoneValidForPlacement();

	//UFUNCTION(BlueprintCallable, Category = "Zone Content")
	//TArray<UActorComponent*> GetZoneObjects();

private:

	// Functions/Methods:

	/** 
	* Check against the positions of objects in this Zone,
	* to find the determine the colour of this Zone's Edges.
	*/
	void DetermineZoneEdgesColour();

	// Properties:

	/** For each Edge of this Zone (4 of them by default). */
	std::vector<FPSLevelGeneratorEdge> ZoneEdges;

	/** 
	* For the colour of this Zone's Edges.
	* This vector is in the order noted above.
	*/
	std::vector<FPSLevelGeneratorEdge::EdgeColour> ZoneEdgeColours;

	/** To hold all of the objects in the Zone. */
	TArray<UStaticMeshComponent*> ZoneObjects;

	/** 
	* For avoid calling the update functions for every
	* call of Tick().
	*/
	float TickTimer;

	// Flags:

	// For if it is valid to place this zone:
	bool ZoneValidForPlacement;

	// Constant Values:

	const FVector DEFAULT_ZONE_EXTENTS = FVector(100.0f, 100.0f, 100.0f);

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
};
