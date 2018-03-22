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

	FVector2D NorthEdgePosition = DEFAULT_NORTH_EDGE_RELATIVE_POSITION;
	FVector2D EastEdgePosition = DEFAULT_EAST_EDGE_RELATIVE_POSITION;
	FVector2D SouthEdgePosition = DEFAULT_SOUTH_EDGE_RELATIVE_POSITION;
	FVector2D WestEdgePosition = DEFAULT_WEST_EDGE_RELATIVE_POSITION;

	NorthEdge->InitialiseEdge(NorthEdgePosition);
	EastEdge->InitialiseEdge(EastEdgePosition);
	SouthEdge->InitialiseEdge(SouthEdgePosition);
	WestEdge->InitialiseEdge(WestEdgePosition);

	EdgeColours = std::vector<EdgeColour>(size_t(DEFAULT_ZONE_EDGE_COUNT));
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

	for (int Iterator = 0; Iterator < ZoneComponents.Num(); Iterator++)
	{
		ZoneObjects.Add(Cast<UStaticMeshComponent>(ZoneComponents[Iterator]));
	}
}

TArray<UFPSLevelGeneratorEdge*> AZone::GetZoneEdges()
{
	return ZoneEdges;
}

std::vector<AZone::EdgeColour> AZone::GetEdgeColours()
{
	return EdgeColours;
}

void AZone::UpdateZoneEdgesColour()
{
	// Keep the edges in check:
	UpdateEdges();

	for (int EdgeCollectionIterator = 0; EdgeCollectionIterator <
		ZoneEdges.Num(); EdgeCollectionIterator++)
	{
		float CurrentEdgeDensity = ZoneEdges[EdgeCollectionIterator]->GetEdgeDensity();
		
		// Grey:
		if (CurrentEdgeDensity > 0 && CurrentEdgeDensity <= GREY_COLOUR_RANGE_UPPER_BOUND)
		{
			EdgeColours[EdgeCollectionIterator] = EdgeColour::Grey;
		}
		// Red:
		else if (CurrentEdgeDensity > GREY_COLOUR_RANGE_UPPER_BOUND &&
			CurrentEdgeDensity <= RED_COLOUR_RANGE_UPPER_BOUND)
		{
			EdgeColours[EdgeCollectionIterator] = EdgeColour::Red;
		}
		// Green:
		else if (CurrentEdgeDensity > RED_COLOUR_RANGE_UPPER_BOUND &&
			CurrentEdgeDensity <= GREEN_COLOUR_RANGE_UPPER_BOUND)
		{
			EdgeColours[EdgeCollectionIterator] = EdgeColour::Green;
		}
		// Blue:
		else if (CurrentEdgeDensity > GREEN_COLOUR_RANGE_UPPER_BOUND &&
			CurrentEdgeDensity <= BLUE_COLOUR_RANGE_UPPER_BOUND)
		{
			EdgeColours[EdgeCollectionIterator] = EdgeColour::Blue;
		}
	}
}

void AZone::UpdateEdges()
{
	for (int Iterator = 0; Iterator < ZoneEdges.Num(); Iterator++)
	{
		ZoneEdges[Iterator]->UpdateEdgeDensity(ZoneObjects);
	}
}

// Update:
void AZone::Tick(float DeltaSeconds)
{
	// Call the update methods when it is time:
	TickTimer += DeltaSeconds;

	if (TickTimer >= UPDATE_FREQUENCY)
	{
		UpdateEdges();
	}	
}
