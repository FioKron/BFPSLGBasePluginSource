// Fill out your copyright notice in the Description page of Project Settings.

#include "Zone.h"
#include "FPSLevelGeneratorEdge.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


// Initialise:
AZone::AZone()
{
	NorthEdge = CreateDefaultSubobject<UFPSLevelGeneratorEdge>("NorthEdge");
	EastEdge = CreateDefaultSubobject<UFPSLevelGeneratorEdge>("EastEdge");
	SouthEdge = CreateDefaultSubobject<UFPSLevelGeneratorEdge>("SouthEdge");
	WestEdge = CreateDefaultSubobject<UFPSLevelGeneratorEdge>("WestEdge");

	FVector2D NorthEdgeCentrePoint = NorthEdge->DEFAULT_NORTH_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D EastEdgeCentrePoint = EastEdge->DEFAULT_EAST_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D SouthEdgeCentrePoint = SouthEdge->DEFAULT_SOUTH_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D WestEdgeCentrePoint = WestEdge->DEFAULT_WEST_EDGE_RELATIVE_CENTRE_POINT;

	NorthEdge->InitialiseEdge(NorthEdgeCentrePoint);
	EastEdge->InitialiseEdge(EastEdgeCentrePoint);
	SouthEdge->InitialiseEdge(SouthEdgeCentrePoint);
	WestEdge->InitialiseEdge(WestEdgeCentrePoint);

	ZoneEdgeColours = std::vector<UFPSLevelGeneratorEdge::EdgeColour>(size_t(DEFAULT_ZONE_EDGE_COUNT));
}

// Initialise what the constructor is not able to:
void AZone::InitialiseZone()
{
	TickTimer = 0.0f;

	ZoneEdges.Add(NorthEdge);
	ZoneEdges.Add(EastEdge);
	ZoneEdges.Add(SouthEdge);
	ZoneEdges.Add(WestEdge);

	// For setting-up zone objects:
	TArray<UActorComponent*> ZoneComponents = GetComponentsByClass(UStaticMeshComponent::StaticClass());
	// To make sure this struct's vector can hold the correct number of components:
	ThisZonesObjects.ZoneObjectsPositionScale = std::vector<ZoneObjectPositionScale>(size_t(ZoneComponents.Num()));

	for (int Iterator = 0; Iterator < ZoneComponents.Num(); Iterator++)
	{
		ZoneObjects.Add(Cast<UStaticMeshComponent>(ZoneComponents[Iterator]));
		ThisZonesObjects.ZoneObjectsPositionScale[Iterator] = 
			ZoneObjectPositionScale(
				ZoneObjects[Iterator]->RelativeLocation,
				ZoneObjects[Iterator]->RelativeScale3D);
	}

	// Now the instance has all the objects in the Zone,
	// it is now possible to determine the proper colour
	// of this Zone's Edges:
	DetermineZoneEdgesColour();
}

TArray<UFPSLevelGeneratorEdge*> AZone::GetZoneEdges()
{
	return ZoneEdges;
}

std::vector<UFPSLevelGeneratorEdge::EdgeColour> AZone::GetZoneEdgeColours()
{
	return ZoneEdgeColours;
}

// Check to see what Zone this is, then change the Edge colours accordingly:
void AZone::DetermineZoneEdgesColour()
{
	if (ThisZonesObjects == WANG_TILE_ONE)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (ThisZonesObjects == WANG_TILE_TWO)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_THREE)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (ThisZonesObjects == WANG_TILE_FOUR)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_FIVE)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_SIX)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (ThisZonesObjects == WANG_TILE_SEVEN)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_EIGHT)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_NINE)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (ThisZonesObjects == WANG_TILE_TEN)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (ThisZonesObjects == WANG_TILE_ELEVEN)
	{
		NorthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge->InitialiseEdgeColour(UFPSLevelGeneratorEdge::EdgeColour::Grey);
	}
}

/**
void AZone::UpdateEdges()
{
	
}
*/

// Update:
void AZone::Tick(float DeltaSeconds)
{
	// Call the update methods when it is time:
	/**
	TickTimer += DeltaSeconds;

	if (TickTimer >= UPDATE_FREQUENCY)
	{
		UpdateEdges();
	}	
	*/
}
