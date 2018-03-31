// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FPSLevelGeneratorEdge.generated.h"

/**
 * 
 */
UCLASS()
class BALANCEDFPSLEVELGENERATOR_API UFPSLevelGeneratorEdge : public UObject
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

	// Functions/Methods:

	/** Default constructor (required by UE4). */
	UFPSLevelGeneratorEdge();

	// Initialisation methods:

	void InitialiseEdge(FVector2D NewEdgeCentrePoint);

	/** EdgeColour is calculated manually for now, the result is parsed into this function. */
	void InitialiseEdgeColour(EdgeColour NewEdgeColour);

	// Get functions:
	const EdgeColour GetEdgeColour();

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
