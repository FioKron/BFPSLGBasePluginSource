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
	TickTimer = 0.0f;
	ZoneValidForPlacement = false;

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

void AZone::SetZoneValidForPlacement(bool NewZoneValidForPlacement)
{
	ZoneValidForPlacement = NewZoneValidForPlacement;
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

bool AZone::GetZoneValidForPlacement()
{
	return ZoneValidForPlacement;
}

// Check to see what Zone this is, then change the Edge colours accordingly:
void AZone::DetermineZoneEdgesColour()
{
	if (Tags.Contains(WANG_TILE_ONE))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_TWO))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_THREE))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_FOUR))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_FIVE))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_SIX))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_SEVEN))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_EIGHT))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_NINE))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
	}
	else if (Tags.Contains(WANG_TILE_TEN))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Red);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
	}
	else if (Tags.Contains(WANG_TILE_ELEVEN))
	{
		NorthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		EastEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		SouthEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Blue);
		WestEdge.InitialiseEdgeColour(FPSLevelGeneratorEdge::EdgeColour::Grey);
	}

	// Make sure to set ZoneEdgeColours up correctly as well:
	ZoneEdgeColours[0] = NorthEdge.GetEdgeColour();
	ZoneEdgeColours[1] = EastEdge.GetEdgeColour();
	ZoneEdgeColours[2] = SouthEdge.GetEdgeColour();
	ZoneEdgeColours[3] = WestEdge.GetEdgeColour();
}

/**
void AZone::UpdateEdges()
{
	
}
*/
