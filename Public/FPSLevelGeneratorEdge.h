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

	/** Default constructor (required by UE4). */
	UFPSLevelGeneratorEdge();

	/** For other classes to update this class's EdgeDensity. */
	void UpdateEdgeDensity(TArray<class UStaticMeshComponent*> ZoneObjects);

	// Initialisation methods:
	void InitialiseEdge(FVector2D NewEdgeCentrePoint);

	// Get functions:
	const float GetEdgeDensity();

private:

	/** For the density of this edge. */
	float EdgeDensity;

	/** 
	* The centre point of this edge, used
	* in the calculations for this Edge's density.
	*/
	FVector2D EdgeCentrePoint;
	
};
