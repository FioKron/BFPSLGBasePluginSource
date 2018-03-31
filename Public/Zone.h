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

	// Structures:

	/** To store the position and scale of any object in the Zone. */
	struct ZoneObjectPositionScale
	{
		// Functions/Methods:

		/** Standard constructor. */
		ZoneObjectPositionScale::ZoneObjectPositionScale(
			FVector NewObjectPosition, FVector NewObjectScale)
		{
			ObjectPosition = NewObjectPosition;
			ObjectScale = NewObjectScale;
		}

		/** Default constructor (required by xmemory0). */
		ZoneObjectPositionScale::ZoneObjectPositionScale()
		{

		}

		// Properties:

		FVector ObjectPosition;
		FVector ObjectScale;
	};

	/** To use for constant values that identify specific Zones. */
	struct ZoneObjectPropertySet
	{
		// Properties:

		std::vector<ZoneObjectPositionScale> ZoneObjectsPositionScale;

		// Operator overloading:

		/** To check for equality between ZoneObjectPropertySets. */
		bool ZoneObjectPropertySet::operator==(const ZoneObjectPropertySet RightHandSide)
		{
			// Set to false if one of the object's FVectors in the vectors of both sets,
			// would not match-up:
			bool SetsAreEqual = true;

			// Compare the properties of each object stored in the struct values for each
			// vector, against the other values:
			for (ZoneObjectPositionScale CurrentZoneObject : ZoneObjectsPositionScale)
			{
				for (ZoneObjectPositionScale ComparisonZoneObject : RightHandSide.ZoneObjectsPositionScale)
				{
					SetsAreEqual = CurrentZoneObject.ObjectPosition == ComparisonZoneObject.ObjectPosition;
					// To skip having to check the rest of the values in each vector against each other:
					if (!SetsAreEqual)
					{
						return SetsAreEqual;
					}
					SetsAreEqual = CurrentZoneObject.ObjectScale == ComparisonZoneObject.ObjectScale;
					if (!SetsAreEqual)
					{
						return SetsAreEqual;
					}
				}
			}

			return SetsAreEqual;
		}
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
	//void UpdateEdges();
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

	std::vector<UFPSLevelGeneratorEdge::EdgeColour> GetZoneEdgeColours();

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
	TArray<UFPSLevelGeneratorEdge*> ZoneEdges;

	/** 
	* For the colour of this Zone's Edges.
	* This vector is in the order noted above.
	*/
	std::vector<UFPSLevelGeneratorEdge::EdgeColour> ZoneEdgeColours;

	/** To hold all of the objects in the Zone. */
	TArray<UStaticMeshComponent*> ZoneObjects;

	/** To check which Zone of the tile-set this is. */
	ZoneObjectPropertySet ThisZonesObjects;

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

	/** These values are used to idenfiy each Zone. */
	const ZoneObjectPropertySet WANG_TILE_ONE = 
	{ std::vector<ZoneObjectPositionScale>
		{ 
			ZoneObjectPositionScale(FVector(5.0f, -45.0f, -50.0f), 
				FVector(0.90f, 0.10f, 1.0f)), 
			ZoneObjectPositionScale(FVector(5.0f, 45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, 0.0f, -50.0f), 
				FVector(0.10f, 1.0f, 1.0f))
		} 
	};
	const ZoneObjectPropertySet WANG_TILE_TWO =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(0.0f, -45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(0.0f, 45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_THREE =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(5.0f, -45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, 0.0f, -50.0f),
				FVector(0.10f, 1.0f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_FOUR =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(-5.0f, -45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(45.0f, 0.0f, -50.0f),
				FVector(0.10f, 1.0f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_FIVE =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(45.0f, 0.0f, -50.0f),
				FVector(0.10f, 1.0f, 1.0f)),
			ZoneObjectPositionScale(FVector(-5.0f, 45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_SIX =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(0.0f, 45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, -5.0f, -50.0f),
				FVector(0.10f, 0.90f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_SEVEN =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(0.2548340f, -0.0001450f, -50.0f),
				FVector(0.10f, 1.30f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_EIGHT =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(-5.0f, -45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(45.0f, -25.0f, -50.0f),
				FVector(0.10f, 0.50f, 1.0f)),
			ZoneObjectPositionScale(FVector(5.0f, 45.0f, -50.0f),
				FVector(0.90f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, 25.0f, -50.0f),
				FVector(0.10f, 0.50f, 1.0f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_NINE =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(0.0f, -45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(45.0f, 0.0f, -50.0f),
				FVector(0.10f, 0.80f, 0.40f)),
			ZoneObjectPositionScale(FVector(0.0f, 45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, 0.0f, -50.0f),
				FVector(0.10f, 0.80f, 0.10f))
		}
	};
	const ZoneObjectPropertySet WANG_TILE_TEN =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(45.0f, 0.0f, -50.0f),
				FVector(0.10f, 1.0f, 1.0f)),
			ZoneObjectPositionScale(FVector(-45.0f, 0.0f, -50.0f),
				FVector(0.10f, 1.0f, 1.0f)),
		}
	};
	const ZoneObjectPropertySet WANG_TILE_ELEVEN =
	{ std::vector<ZoneObjectPositionScale>
		{
			ZoneObjectPositionScale(FVector(0.0f, -45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(45.0f, 0.0f, -50.0f),
				FVector(0.10f, 0.80f, 0.40f)),
			ZoneObjectPositionScale(FVector(0.0f, 45.0f, -50.0f),
				FVector(1.0f, 0.10f, 1.0f)),
			ZoneObjectPositionScale(FVector(0.0f, 0.0f, -50.0f),
				FVector(0.10f, 0.80f, 0.40f)),
			ZoneObjectPositionScale(FVector(-45.0f, 0.0f, -50.0f),
				FVector(0.10f, 0.80f, 0.40f)),
		}
	};
};
