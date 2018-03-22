// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object.h"


/**
	This is the root class for Edges and Zones, as per the
	method described in the report.
*/
class BALANCEDFPSLEVELGENERATOR_API Area : UObject
{
public:

	// Functions/Methods

	/** Standard constructor. */
	Area();

	/** Standard destructor. */
	~Area();

protected:

	// Properties:
	
	/** The position of this Area in the World. */
	FVector CurrentAreaCoordinates;

	/** 
		The width, height and depth of this Area
		in the World.
	*/
	FVector CurrentAreaExtents;

	/** 
		The average object density for this 
		Area (in objects-per-Unreal-Unit-
		Cube).
	*/
	float CurrentAreaObjectDensity;
};
