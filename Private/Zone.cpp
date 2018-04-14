// Fill out your copyright notice in the Description page of Project Settings.

#include "Zone.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


// Initialise:
AZone::AZone()
{
	// Make sure this zone is not cleaned-up before it's used:
	AddToRoot();
	/**
	// Make sure these Edges are rooted (so they are not collected by
	// the Garbage Collector):
	NorthEdge.SetFlags(EObjectFlags::RF_MarkAsRootSet);
	NorthEdge.AddToRoot();
	EastEdge.SetFlags(EObjectFlags::RF_MarkAsRootSet);
	EastEdge.AddToRoot();
	SouthEdge.SetFlags(EObjectFlags::RF_MarkAsRootSet);
	SouthEdge.AddToRoot();
	WestEdge.SetFlags(EObjectFlags::RF_MarkAsRootSet);
	WestEdge.AddToRoot();
	*/

	FVector2D NorthEdgeCentrePoint = NorthEdge.DEFAULT_NORTH_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D EastEdgeCentrePoint = EastEdge.DEFAULT_EAST_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D SouthEdgeCentrePoint = SouthEdge.DEFAULT_SOUTH_EDGE_RELATIVE_CENTRE_POINT;
	FVector2D WestEdgeCentrePoint = WestEdge.DEFAULT_WEST_EDGE_RELATIVE_CENTRE_POINT;

	NorthEdge.InitialiseEdge(NorthEdgeCentrePoint);
	EastEdge.InitialiseEdge(EastEdgeCentrePoint);
	SouthEdge.InitialiseEdge(SouthEdgeCentrePoint);
	WestEdge.InitialiseEdge(WestEdgeCentrePoint);

	ZoneEdgeColours = std::vector<FPSLevelGeneratorEdge::EdgeColour>(size_t(DEFAULT_ZONE_EDGE_COUNT));
}

// Initialise what the constructor is not able to:
void AZone::InitialiseZone()
{
	ZoneEdges.push_back(NorthEdge);
	ZoneEdges.push_back(EastEdge);
	ZoneEdges.push_back(SouthEdge);
	ZoneEdges.push_back(WestEdge);

	// For setting-up zone objects:
	TArray<UActorComponent*> ZoneComponents = GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (int Iterator = 0; Iterator < ZoneComponents.Num(); Iterator++)
	{
		ZoneObjects.Add(Cast<UStaticMeshComponent>(ZoneComponents[Iterator]));
	}

	// Now the instance has all the objects in the Zone,
	// it is now possible to determine the proper colour
	// of this Zone's Edges:
	DetermineZoneEdgesColour();
}

std::vector<FPSLevelGeneratorEdge> AZone::GetZoneEdges()
{
	return ZoneEdges;
}

std::vector<FPSLevelGeneratorEdge::EdgeColour> AZone::GetZoneEdgeColours()
{
	// Make sure to set ZoneEdgeColours up correctly as well:
	ZoneEdgeColours[0] = NorthEdge.GetEdgeColour();
	ZoneEdgeColours[1] = EastEdge.GetEdgeColour();
	ZoneEdgeColours[2] = SouthEdge.GetEdgeColour();
	ZoneEdgeColours[3] = WestEdge.GetEdgeColour();
	return ZoneEdgeColours;
}

// Check to see what Zone this is, then change the Edge colours accordingly:
void AZone::DetermineZoneEdgesColour()
{
	if (Tags.Contains(WANG_TILE_ONE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_TWO))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_THREE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_FOUR))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_FIVE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_SIX))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_SEVEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_EIGHT))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_NINE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_TEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_ELEVEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Green, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Grey);
	}
	else if (Tags.Contains(WANG_TILE_TWELVE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_THIRTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_FOURTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_FIFTHTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_SIXTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_SEVENTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_EIGHTTEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Grey,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Green,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_NINETEEN))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_TWENTY))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_TWENTY_ONE))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Blue,
			FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_TWENTY_TWO))
	{
		InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Red, FPSLevelGeneratorEdge::EdgeColour::Red,
			FPSLevelGeneratorEdge::EdgeColour::Blue);
	}

	// Make sure to set ZoneEdgeColours up correctly as well:
	ZoneEdgeColours[0] = NorthEdge.GetEdgeColour();
	ZoneEdgeColours[1] = EastEdge.GetEdgeColour();
	ZoneEdgeColours[2] = SouthEdge.GetEdgeColour();
	ZoneEdgeColours[3] = WestEdge.GetEdgeColour();
}

// Initialise this Zone's Edge colours:
void AZone::InitialiseZoneEdgeColours(FPSLevelGeneratorEdge::EdgeColour NorthEdgeColour,
	FPSLevelGeneratorEdge::EdgeColour EastEdgeColour, FPSLevelGeneratorEdge::EdgeColour SouthEdgeColour,
	FPSLevelGeneratorEdge::EdgeColour WestEdgeColour)
{
	NorthEdge.InitialiseEdgeColour(NorthEdgeColour);
	EastEdge.InitialiseEdgeColour(EastEdgeColour);
	SouthEdge.InitialiseEdgeColour(SouthEdgeColour);
	WestEdge.InitialiseEdgeColour(WestEdgeColour);
}

/**
void AZone::UpdateEdges()
{
	
}
*/
