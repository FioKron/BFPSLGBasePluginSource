// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLevelGeneratorEdge.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"

// Required by UE4 for UObject sub-classes:
UFPSLevelGeneratorEdge::UFPSLevelGeneratorEdge()
{

}

void UFPSLevelGeneratorEdge::UpdateEdgeDensity(TArray<UStaticMeshComponent*> ZoneObjects)
{
	// To allow validation before updating the EdgeDensity:
	float LocalEdgeDensity = 0.0f;
	// For all Actors in the zone:
	float TotalZoneVolume = 0.0f;
	// For the distance between these Actors and the centre 
	// point of the edge:
	float MeanDistance = 0.0f;

	// Assuming that the ZoneObjects are varients of the default engine
	// cube (100 X 100 X 100), accounting for scale:
	const FVector DEFAULT_COMPONENT_VOLUME = FVector(100.0f, 100.0f, 100.0f);
	for (int Iterator = 0; Iterator < ZoneObjects.Num(); Iterator++)
	{
		FVector ScaledComponentDimensions = DEFAULT_COMPONENT_VOLUME * ZoneObjects[Iterator]->GetComponentScale();
		TotalZoneVolume += ScaledComponentDimensions.X * ScaledComponentDimensions.Y * ScaledComponentDimensions.Z;
		MeanDistance += FVector::Dist(FVector(EdgeCentrePoint, ZoneObjects[Iterator]->RelativeLocation.Z), ZoneObjects[Iterator]->RelativeLocation);
	}

	// For the mean (average) value:
	MeanDistance /= ZoneObjects.Num();

	LocalEdgeDensity = TotalZoneVolume / MeanDistance;
	EdgeDensity = LocalEdgeDensity;
}

void UFPSLevelGeneratorEdge::InitialiseEdge(FVector2D NewEdgeCentrePoint)
{
	EdgeCentrePoint = NewEdgeCentrePoint;
}

const float UFPSLevelGeneratorEdge::GetEdgeDensity()
{
	return EdgeDensity;
}
