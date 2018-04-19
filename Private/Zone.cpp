// Fill out your copyright notice in the Description page of Project Settings.

#include "Zone.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"


// Initialise:
AZone::AZone()
{
	DefensivenessCoefficient = 0.0f;
	FlankingCoefficient = 0.0f;
	DispersionCoefficient = 0.0f;
}

// Initialise what the constructor is not able to:
void AZone::InitialiseZone()
{
	// For setting-up zone objects:
	TArray<UActorComponent*> ZoneComponents = GetComponentsByClass(UStaticMeshComponent::StaticClass());

	for (int Iterator = 0; Iterator < ZoneComponents.Num(); Iterator++)
	{
		ZoneObjects.Add(Cast<UStaticMeshComponent>(ZoneComponents[Iterator]));
	}

	// Now the instance has all the objects in the Zone,
	// it is now possible to determine the proper colour
	// of this Zone's Edges:
	DetermineInitialZoneValues();
}

// Get functions:

float AZone::GetDefensivenessCoefficient()
{
	return DefensivenessCoefficient;
}

float AZone::GetFlankingCoefficient()
{
	return FlankingCoefficient;
}

float AZone::GetDispersonCoefficient()
{
	return DispersionCoefficient;
}

// Check to see what Zone this is, then set this Zone's values accordingly:
void AZone::DetermineInitialZoneValues()
{
	// Dispersion Coefficient is precise to 2 decimal places:
	if (Tags.Contains(WANG_TILE_ONE))
	{
		DispersionCoefficient = 0.230f;
	}
	else if (Tags.Contains(WANG_TILE_TWO))
	{
		DispersionCoefficient = 0.50f;
	}
	else if (Tags.Contains(WANG_TILE_THREE))
	{
		DispersionCoefficient = 0.250f;
	}
	else if (Tags.Contains(WANG_TILE_FOUR))
	{
		DispersionCoefficient = 0.250f;
	}
	else if (Tags.Contains(WANG_TILE_FIVE))
	{
		DispersionCoefficient = 0.250f;
	}
	else if (Tags.Contains(WANG_TILE_SIX))
	{
		DispersionCoefficient = 0.250f;
	}
	else if (Tags.Contains(WANG_TILE_SEVEN))
	{
		DispersionCoefficient = 1.0f;
	}
	else if (Tags.Contains(WANG_TILE_EIGHT))
	{
		DispersionCoefficient = 0.140f;
	}
	else if (Tags.Contains(WANG_TILE_NINE))
	{
		DispersionCoefficient = 0.130f;
	}
	else if (Tags.Contains(WANG_TILE_TEN))
	{
		DispersionCoefficient = 0.50f;
	}
	else if (Tags.Contains(WANG_TILE_ELEVEN))
	{
		DispersionCoefficient = 0.150f;
	}
	else if (Tags.Contains(WANG_TILE_TWELVE))
	{
		DispersionCoefficient = 1.0f;
	}
	else if (Tags.Contains(WANG_TILE_THIRTEEN))
	{
		DispersionCoefficient = 0.140f;
	}
	else if (Tags.Contains(WANG_TILE_FOURTEEN))
	{
		DispersionCoefficient = 0.240f;
	}
	else if (Tags.Contains(WANG_TILE_FIFTHTEEN))
	{
		DispersionCoefficient = 0.240f;
	}
	else if (Tags.Contains(WANG_TILE_SIXTEEN))
	{
		DispersionCoefficient = 0.240f;
	}
	else if (Tags.Contains(WANG_TILE_SEVENTEEN))
	{
		DispersionCoefficient = 0.170f;
	}
	else if (Tags.Contains(WANG_TILE_EIGHTTEEN))
	{
		DispersionCoefficient = 0.150f;
	}
	else if (Tags.Contains(WANG_TILE_NINETEEN))
	{
		DispersionCoefficient = 1.0f;
	}
	else if (Tags.Contains(WANG_TILE_TWENTY))
	{
		DispersionCoefficient = 1.0f;
	}
	else if (Tags.Contains(WANG_TILE_TWENTY_ONE))
	{
		DispersionCoefficient = 1.0f;
	}
	else if (Tags.Contains(WANG_TILE_TWENTY_TWO))
	{
		DispersionCoefficient = 1.0f;
	}
}

// As per the equations detailed in the report:
void AZone::DetermineDefensivenessAndFlankingCoefficients(float SurroundingZones,
	float AdjacentZones)
{
	FlankingCoefficient = 1.0f - (AdjacentZones / SurroundingZones);
	// For determining the Defensiveness Coefficient:
	float ZoneObjectVolume = ZoneObjects.Num() / HIGHEST_ZONE_VOLUME;

	// For finding the path density:
	std::vector<float> TouchingEdgeCount;

	// 7 Edges are touching 2 of the Edges that matter in this case (for 2 or 3 adjacent Zones):
	if (AdjacentZones != 4.0f)
	{
		TouchingEdgeCount.push_back(7.0f);
	}
	
	if (AdjacentZones == 2.0f)
	{		
		InitialiseDefensivenessCoefficientCalculations(TouchingEdgeCount, ZoneObjectVolume, AdjacentZones);
		return;
	}
	else if (AdjacentZones == 3.0f || AdjacentZones == 4.0f)
	{
		// 12 Edges are also considered to be touching the East Edge in this case, or
		// 12 Edges are considered to be touching all 4 Edges:
		TouchingEdgeCount.push_back(12.0f);
		InitialiseDefensivenessCoefficientCalculations(TouchingEdgeCount, ZoneObjectVolume, AdjacentZones);
	}
}

// Run the calculations to determine the Defensiveness Coefficient:
void AZone::InitialiseDefensivenessCoefficientCalculations(std::vector<float> TouchingEdgeCount,
	float ZoneObjectVolume, float AdjacentZones)
{
	// As this will be decremented, then the absolute value will be obtained from this: 
	float PathDensity = HIGHEST_ZONE_VOLUME;
	// Whether to switch between the values in TouchingEdgeCount:
	bool FlipFlopRequired = false;

	// 2 or 4 adjacent Zones:
	if (TouchingEdgeCount.size() == 1)
	{
		FindNonAbsolutePathDensity(PathDensity, FlipFlopRequired, TouchingEdgeCount, AdjacentZones);
	}
	// 3 adjacent Zones:
	else
	{
		// To swap between the first and last Edge-Volume Values:
		FlipFlopRequired = true;
		FindNonAbsolutePathDensity(PathDensity, FlipFlopRequired, TouchingEdgeCount, AdjacentZones);
	}
}

// Perform the repetitive calculations first, before the next step:
void AZone::FindNonAbsolutePathDensity(float& PathDensity, bool IsFlipFlopRequired, 
	std::vector<float> TouchingEdgeCount, float AdjacentZones)
{
	// For switching between values in the touching edge-count collection:
	int FlipFlopIndex = 0;

	for (int SummationCounter = 0; SummationCounter < AdjacentZones; SummationCounter++)
	{
		PathDensity -= TouchingEdgeCount[FlipFlopIndex];

		// Flip between the positions of this collection (if this is required):
		if (FlipFlopIndex == 0 && IsFlipFlopRequired)
		{
			FlipFlopIndex++;
		}
		else if (FlipFlopIndex == 1 && IsFlipFlopRequired)
		{
			FlipFlopIndex--;
		}
	}
}

// The last set of calculations to determine the Defensiveness Coefficient:
void AZone::FindDefensivenessCoefficient(float ZoneObjectVolume, float& PathDensity)
{
	// The absolute value is what matters here (for comparison):
	PathDensity = abs(PathDensity);
	PathDensity /= HIGHEST_ZONE_VOLUME;
	DefensivenessCoefficient = (ZoneObjectVolume + PathDensity) / 2.0f;
}

