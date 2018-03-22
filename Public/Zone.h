// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "UObject/NoExportTypes.h"
#include "vector" // For misc. collections.
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
	// Enumerations:
	
	enum EdgeColour
	{
		Grey,
		Red,
		Green,
		Blue
	};
	
	// Properties:

	/** These represent the Edges of this zone (to be put into the ZoneEdges Array). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone perimeter")
	class UFPSLevelGeneratorEdge* NorthEdge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone perimeter")
	UFPSLevelGeneratorEdge* EastEdge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone perimeter")
	UFPSLevelGeneratorEdge* SouthEdge;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone perimeter")
	UFPSLevelGeneratorEdge* WestEdge;

	// Functions/Methods:

	/** Default constructor (required by UE4). */
	AZone();

	/** For proper initialisation. */
	void InitialiseZone();

	/** For updating */
	void UpdateEdges();
	void UpdateZoneEdgesColour();
	void Tick(float DeltaSeconds)override;
	
	// Get functions:

	/** 
	* Zone Edge order:
	* First element: North Edge.
	* Second element: East Edge.
	* Third element: South Edge.
	* Fourth element: West Edge.
	*/
	UFUNCTION(BlueprintCallable, Category = "Zone Perimeter")
	TArray<UFPSLevelGeneratorEdge*> GetZoneEdges();

	std::vector<EdgeColour> GetEdgeColours();

	//UFUNCTION(BlueprintCallable, Category = "Zone Content")
	//TArray<UActorComponent*> GetZoneObjects();

private:

	// Properties:

	/** For each Edge of this Zone (4 of them by default). */
	TArray<UFPSLevelGeneratorEdge*> ZoneEdges;

	/** 
	* For the colour of this Zone's Edges.
	* This TArray is in the order noted above.
	*/
	std::vector<EdgeColour> EdgeColours;

	/** To hold all of the objects in the Zone. */
	TArray<UStaticMeshComponent*> ZoneObjects;

	/** 
	* For avoid calling the update functions for every
	* call of Tick().
	*/
	float TickTimer;

	// Constant Values:

	const FVector DEFAULT_ZONE_EXTENTS = FVector(100.0f, 100.0f, 100.0f);

	/** 
	* For how often the update functions should
	* be called in Tick().
	*/
	const float UPDATE_FREQUENCY = 1000.0f;

	/** For the default ZoneEdge properties (during initialisation). */
	const int DEFAULT_ZONE_EDGE_COUNT = 4;

	/** For relative positions. */
	const FVector2D DEFAULT_NORTH_EDGE_RELATIVE_POSITION = FVector2D(0.0f, -45.0f);
	const FVector2D DEFAULT_EAST_EDGE_RELATIVE_POSITION = FVector2D(45.0f, 0.0f);
	const FVector2D DEFAULT_SOUTH_EDGE_RELATIVE_POSITION = FVector2D(0.0f, 45.0f);
	const FVector2D DEFAULT_WEST_EDGE_RELATIVE_POSITION = FVector2D(-45.0f, 0.0f);

	/** For determining the colour of an Edge. */
	const int GREY_COLOUR_RANGE_UPPER_BOUND = 2500;
	const int RED_COLOUR_RANGE_UPPER_BOUND = 5000;
	const int GREEN_COLOUR_RANGE_UPPER_BOUND = 7500;
	const int BLUE_COLOUR_RANGE_UPPER_BOUND = 10000;
};
