// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLevelGeneratorEdge.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Editor/UnrealEd/Public/Editor.h"

// Required by UE4 for UObject sub-classes:
UFPSLevelGeneratorEdge::UFPSLevelGeneratorEdge()
{

}

// Initialisation methods:
void UFPSLevelGeneratorEdge::InitialiseEdge(FVector2D NewEdgeCentrePoint)
{
	EdgeCentrePoint = NewEdgeCentrePoint;
}

void UFPSLevelGeneratorEdge::InitialiseEdgeColour(EdgeColour NewEdgeColour)
{
	ThisEdgeColour = NewEdgeColour;
}

// Get methods:
const UFPSLevelGeneratorEdge::EdgeColour UFPSLevelGeneratorEdge::GetEdgeColour()
{
	return ThisEdgeColour;
}
