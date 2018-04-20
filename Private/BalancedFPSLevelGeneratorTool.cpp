// Fill out your copyright notice in the Description page of Project Settings.

#include "BalancedFPSLevelGeneratorTool.h"
#include "MessageDialog.h"
#include "Engine/World.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h "
#include "Editor/UnrealEd/Public/FileHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Engine/Blueprint.h"
// For access to the GEditor object:
#include "Editor/UnrealEd/Public/Editor.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Classes/Engine/PointLight.h"
#include "Runtime/Core/Public//Math/UnrealMathUtility.h"
#include "Runtime/Core/Public/HAL/Platform.h"

// For pseudo-random number generation:
#include "random"
#include "time.h"

// For other STL Containers:
#include <set>


// Initialise:
UBalancedFPSLevelGeneratorTool::UBalancedFPSLevelGeneratorTool()
{
	ApplicableZoneIndicesForWangTile2 = { 3, 4, 6, 7, 9, 10, 11, 14 };
	ApplicableZoneIndicesForWangTile10 = { 4, 5, 6, 11, 12, 15, 16, 18 };

	WallPanelBlueprintAsset = ConstructorHelpers::FObjectFinder<UBlueprint>(
		TEXT("Blueprint'/Game/BalancedFPSLevelGeneratorAssets/Blueprints/WallPanel.WallPanel'"))
		.Object;

	for (int ZoneBlueprintCounter = 1; ZoneBlueprintCounter < TOTAL_ZONE_BLUEPRINT_COUNT + 1;
		ZoneBlueprintCounter++)
	{
		FString IncrementalPathString = "Blueprint'/Game/BalancedFPSLevelGeneratorAssets/Blueprints/WangTiles/";
		IncrementalPathString.Append(FString("WangTile") + FString::FromInt(ZoneBlueprintCounter) + FString(".WangTile") +
			FString::FromInt(ZoneBlueprintCounter) + FString("'"));
		LevelZoneTileBlueprints.Add(ConstructorHelpers::FObjectFinder<UBlueprint>(*IncrementalPathString)
			.Object);
	}

	DefaultRelativePanelScale = FVector(1.0f, 1.0f, 1.0f);
	LevelExtents = FVector2D(300.0f, 300.0f);
	LevelGenerationStartPoint = FVector(0.0f, 0.0f, 0.0f);

	HelpText = FText::FromString(
		"Change properties, before generating a level" 
		" in this dialog panel.");
}

/**
	Generate the level.
	Using https://docs.unrealengine.com/latest/INT/API/Runtime/Engine/Engine/UWorld/index.html
	for reference, as well as https://answers.unrealengine.com/questions/440347/create-a-new-level-from-c-code.html
*/

void UBalancedFPSLevelGeneratorTool::GenerateLevel()
{
	// Initialise the level generation area first...
	InitialiseLevelGenerationArea();		
}

// These functions handle initialisation of the level generation area:
void UBalancedFPSLevelGeneratorTool::InitialiseLevelGenerationArea()
{
	// Encapsulate this level generation area first...
	EncapsulateLevelGenerationArea();

	// ...add a light source to it...
	AddLightSourceToLevelGenerationArea();

	// ...then the level Zones can be added to it:
	AddZonesToLevelGenerationArea();
}

void UBalancedFPSLevelGeneratorTool::EncapsulateLevelGenerationArea()
{
	// For testing:
	int FrontFaceLoopCycleCount = 0;
	int RightFaceLoopCycleCount = 0;
	int BackFaceLoopCycleCount = 0;
	int LeftFaceLoopCycleCount = 0;

	
	// Offset by DEFAULT_ENCAPSULATION_OFFSET, to allow Zones to fill in these gaps:
	// Top and bottom faces:
	for (float CurrentYPosition = LevelGenerationStartPoint.Y;
		CurrentYPosition < LevelExtents.Y; CurrentYPosition +=
		DEFAULT_TILE_WIDTH)
	{
		for (float CurrentXPosition = LevelGenerationStartPoint.X;
			CurrentXPosition < LevelExtents.X; CurrentXPosition +=
			DEFAULT_TILE_WIDTH)
		{	
			// For the bottom tile first...
			SpawnTopOrBottomWallPanelTileForCurrentPosition(FVector(CurrentXPosition, CurrentYPosition, 0.0f), false);
		
			// ...then the top tile:
			SpawnTopOrBottomWallPanelTileForCurrentPosition(FVector(CurrentXPosition, CurrentYPosition, 0.0f), true);
		}
	}
}

// For either the top of bottom faces, of the level-generation area encapsulation geometry:
void UBalancedFPSLevelGeneratorTool::SpawnTopOrBottomWallPanelTileForCurrentPosition(FVector PanelTilePosition, bool IsTopFaceTile)
{
	// For each wall panel to use in initialisation:
	static AActor* WallPanelActor;

	// It seems rotation has been shuffled to the left, with a 
	// wrap around (Z for Y, Y for X etc.) in UE4:
	FRotator TopBottomFaceRotation = FRotator(0.0f, 0.0f, 90.0f);
	FTransform LevelPanelTransform = FTransform();

	if (IsTopFaceTile)
	{
		// The top face is on the 95th XY-plane:
		PanelTilePosition.Z += DEFAULT_TILE_HEIGHT - (DEFAULT_ENCAPSULATION_OFFSET / 2);
	}
	// For a bottom face tile:
	else
	{
		// The bottom face is on the -10th XY-plane:
		PanelTilePosition.Z -= DEFAULT_ENCAPSULATION_OFFSET;
	}

	LevelPanelTransform = FTransform(TopBottomFaceRotation.Quaternion(), PanelTilePosition, DefaultRelativePanelScale);

	WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
		WallPanelBlueprintAsset, LevelPanelTransform, false);
	WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
}

void UBalancedFPSLevelGeneratorTool::AddLightSourceToLevelGenerationArea()
{
	// Put a point-light at the centre of the now encapsulated level generation area:
	FTransform DefaultLightSourceTransform = FTransform(FRotator::ZeroRotator.Quaternion(), FVector(LevelGenerationStartPoint.X + 0.50f
		* LevelExtents.X, LevelGenerationStartPoint.Y + 0.50f * LevelExtents.Y, LevelGenerationStartPoint.Z + 0.50f * DEFAULT_TILE_HEIGHT),
		FVector(1.0f));
	APointLight* DefaultLightSource = Cast<APointLight>(GEditor->AddActor(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
		APointLight::StaticClass(), DefaultLightSourceTransform));
}

// Now zones can be added to it (Wang Tiles):
void UBalancedFPSLevelGeneratorTool::AddZonesToLevelGenerationArea()
{
	// For each Zone to use in initialisation:
	static AActor* ZoneTile;

	// Set-up the relative corner positions (now that the bounds of the level-generation area are known):
	TopLeftCorner = FVector2D(LevelGenerationStartPoint.X + ZONE_POSITION_OFFSET.X,
		LevelGenerationStartPoint.Y + ZONE_POSITION_OFFSET.Y);
	TopRightCorner = FVector2D(LevelExtents.X - ZONE_POSITION_OFFSET.X,
		LevelGenerationStartPoint.Y + ZONE_POSITION_OFFSET.Y);
	BottomRightCorner = FVector2D(LevelExtents.X - ZONE_POSITION_OFFSET.X,
		LevelExtents.Y - ZONE_POSITION_OFFSET.Y);
	BottomLeftCorner = FVector2D(LevelGenerationStartPoint.X + ZONE_POSITION_OFFSET.X,
		LevelExtents.Y - ZONE_POSITION_OFFSET.Y);

	// For the Zone Blueprints, as Actors:
	TArray<AActor*> ActorZones;
	UGameplayStatics::GetAllActorsOfClass(GEditor->GetEditorWorldContext()
		.World()->GetCurrentLevel(), AZone::StaticClass(), ActorZones);

	// Remove all the Zone Blueprints that have no 'TileSpawnBlueprint' tag:
	for (int ActorZonesIterator = 0; ActorZonesIterator < ActorZones.Num() - 1; ActorZonesIterator++)
	{
		if (ActorZones[ActorZonesIterator]->Tags.Find(TILE_SPAWN_BLUEPRINT_TAG) == INDEX_NONE)
		{
			ActorZones.RemoveAt(ActorZonesIterator, 1, true);
			// As this item has been removed:
			ActorZonesIterator--;
		}
	}
	
	// Store the zones:
	for (int ActorZonesCounter = 0; ActorZonesCounter < ActorZones.Num(); ActorZonesCounter++)
	{
		// Initialise here as well:
		LevelZones.Add(Cast<AZone, AActor>(ActorZones[ActorZonesCounter]));
		LevelZones[ActorZonesCounter]->InitialiseZone();
	}

	// The main loop to place the zones:
	
	// Work backwards from the last row:
	for (float CurrentYPosition = LevelGenerationStartPoint.Y + LevelExtents.Y;
		CurrentYPosition > LevelGenerationStartPoint.Y; CurrentYPosition -=
		DEFAULT_TILE_WIDTH)
	{
		for (float CurrentXPosition = LevelGenerationStartPoint.X;
			CurrentXPosition < LevelExtents.X; CurrentXPosition +=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(CurrentXPosition,
				CurrentYPosition, DEFAULT_TILE_Z_POSITION);
			FRotator ZoneRotation = FRotator(0.0f, 0.0f, 0.0f);

			FTransform LevelZoneTransform = FTransform(ZoneRotation.Quaternion(), CurrentPosition, DEFAULT_ZONE_SCALE);

			// Offset the X and Y components of the CurrentPosition:
			LevelZoneTransform.SetComponents(LevelZoneTransform.GetRotation(), FVector(LevelZoneTransform.GetLocation().X +
				ZONE_POSITION_OFFSET.X, LevelZoneTransform.GetLocation().Y - ZONE_POSITION_OFFSET.Y, LevelZoneTransform.GetLocation().Z),
				LevelZoneTransform.GetScale3D());

			// INVALID ACCESS OPERATION OCCURS HERE:
			UBlueprint* ZoneTileBlueprint = GetSuitableZoneTile(FVector2D(LevelZoneTransform.GetLocation()));

			ZoneTile = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
				ZoneTileBlueprint, LevelZoneTransform, false);
			// Nullify the ZoneTileBlueprint, so that it is set again in the next iterator of this loop:
			ZoneTileBlueprint = nullptr;

			// Sanity check:
			if (ZoneTile)
			{
				ZoneTile->ExecuteConstruction(LevelZoneTransform, nullptr, nullptr, true);
			}
		}
	}
	
	// Clear up the placed level Zones for the next level generated:
	PlacedLevelZones.Empty();
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetSuitableZoneTile(FVector2D CurrentPlacementPosition)
{	
	// For the index to find the target Zone, from the array of Zones:
	int ZoneChoice = -1;

	// Flags:

	bool PlacementInCorner = false;
	bool PlacementAlongEdge = false;

	// Then fill it with all of the zones (these will be narrowed down to the
	// final choice for this space, later):
	ZoneSubSet = LevelZones;

	// Check to see if the function can return a value here:
	
	// For choosing from a particular set of Zones:
	std::default_random_engine RNG;
	std::uniform_int_distribution<int> RandomDistribution;
	RNG.seed(time(NULL));

	// ADD MORE COMBINATIONS!! GWAGIU*WA
	// RESOLVE ISSUES WITH PLACEMENT OF ZONES IN THE CORNERS
	// OF THE LEVEL GENERATION AREA!@GAWI

	// Zone 3:
	if (CurrentPlacementPosition == TopLeftCorner)
	{		
		ZoneChoice = ZONE_THREE_INDEX;
		PlacementInCorner = true;
	}
	// Zone 4:
	else if (CurrentPlacementPosition == TopRightCorner)
	{
		ZoneChoice = ZONE_FOUR_INDEX;
		PlacementInCorner = true;
	}
	// Zone 5:
	else if (CurrentPlacementPosition == BottomRightCorner)
	{
		ZoneChoice = ZONE_FIVE_INDEX;
		PlacementInCorner = true;
	}
	// Zone 6:
	else if (CurrentPlacementPosition == BottomLeftCorner)
	{
		ZoneChoice = ZONE_SIX_INDEX;
		PlacementInCorner = true;
	}

	// If this Zone is to be placed against an edge of
	// the level generation area, then the tile will 
	// have to match 'pure' wall 'edges', as well as
	// the Edge colour of any other adjacent Zones:

	// If this area has already been deemed to be a corner,
	// then skip these checks:

	// This Zone will be placed along an edge of the level generation area:
	if (!PlacementInCorner)
	{
		// North level-generation area 'edge':
		if (CurrentPlacementPosition.Y == LevelGenerationStartPoint.Y + ZONE_POSITION_OFFSET.Y)
		{
			ZoneChoice = ZONE_NINETEEN_INDEX;
			PlacementAlongEdge = true;
		}

		// East level-generation area 'edge':
		if (CurrentPlacementPosition.X == LevelExtents.X - ZONE_POSITION_OFFSET.X)
		{
			ZoneChoice = ZONE_TWENTY_INDEX;
			PlacementAlongEdge = true;
		}

		// South level-generation area 'edge':
		if (CurrentPlacementPosition.Y == LevelExtents.Y - ZONE_POSITION_OFFSET.Y)
		{
			ZoneChoice = ZONE_TWENTY_ONE_INDEX;
			PlacementAlongEdge = true;
		}

		// West level-generation area 'edge':
		if (CurrentPlacementPosition.X == LevelGenerationStartPoint.X + ZONE_POSITION_OFFSET.X)
		{
			ZoneChoice = ZONE_TWENTY_TWO_INDEX;
			PlacementAlongEdge = true;
		}
	}
	
	// A Zone will be placed in a corner or along an Edge of the level-generation area
	// (so determine its defensive and flanking coefficients' respectivly):
	if (PlacementInCorner)
	{
		ZoneSubSet[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(3.0f, 2.0f);
		return GetTargetZone(ZoneChoice);
	}

	if (PlacementAlongEdge)
	{
		ZoneSubSet[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(5.0f, 3.0f);
		return GetTargetZone(ZoneChoice);
	}

	// Check If any Zones have been placed:
	for (int PlacedZonesIterator = 0; PlacedZonesIterator < PlacedLevelZones.Num();
		PlacedZonesIterator++)
	{
		if (PlacedLevelZones.Num() == 0)
		{
			// No zones have been placed:
			break;
		}

		// There is a tile to the west:
		if (PlacedLevelZones[PlacedZonesIterator]->GetActorLocation().X ==
			(CurrentPlacementPosition.X - DEFAULT_TILE_WIDTH + ZONE_POSITION_OFFSET.X))
		{
			ZoneChoice = GetZoneConsideringCoefficients(PlacedZonesIterator,
				ZoneAdjacencyDirection::Westwards);
		}

		// There is a tile to the south:
		if (PlacedLevelZones[PlacedZonesIterator]->GetActorLocation().Y ==
			(CurrentPlacementPosition.Y - DEFAULT_TILE_HEIGHT + ZONE_POSITION_OFFSET.Y))
		{
			ZoneChoice = GetZoneConsideringCoefficients(PlacedZonesIterator,
				ZoneAdjacencyDirection::Southwards);
		}

		if (ZoneChoice != -1)
		{
			// Initialise Defensiveness and Flanking Coefficients here:
			ZoneSubSet[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(8.0f, 4.0f);
			return GetTargetZone(ZoneChoice);
		}
	}

	if (ZoneChoice == -1)
	{
		// Test for now:
		ZoneChoice = GetZoneConsideringCoefficients(0,
				ZoneAdjacencyDirection::Westwards);	
		return GetTargetZone(ZoneChoice);
	}

	// Flow should never reach this point:

	return nullptr;
}

// The coefficients will be considered here, for the choice of Zone:
int UBalancedFPSLevelGeneratorTool::GetZoneConsideringCoefficients(int ZoneToCompareTo, ZoneAdjacencyDirection PlacedZoneAdjacency)
{
	// For the Coefficients to consider:
	float ConsideredZoneDefensivenessCoefficient = PlacedLevelZones[ZoneToCompareTo]->GetDefensivenessCoefficient();
	float ConsideredZoneFlankingCoefficient = PlacedLevelZones[ZoneToCompareTo]->GetFlankingCoefficient();
	float ConsideredZoneDispersionCoefficient = PlacedLevelZones[ZoneToCompareTo]->GetDispersonCoefficient();

	// Check through all of the Zones to find a suitable Zone for placement:
	for (int ZoneIterator = 0; ZoneIterator < ZoneSubSet.Num() - 1;
		ZoneIterator++)
	{
		// To populate with applicable Zone indices:
		std::vector<int> ApplicableZoneIndices;

		float CurrentZoneDefensivenessCoefficient = ZoneSubSet[ZoneIterator]->GetDefensivenessCoefficient();
		float CurrentZoneFlankingCoefficient = ZoneSubSet[ZoneIterator]->GetFlankingCoefficient();
		float CurrentZoneDispersionCoefficient = ZoneSubSet[ZoneIterator]->GetDispersonCoefficient();

		// Consider dispersion first (of the Zone already placed in the level):

		/**
		// A Zone is to be placed in an adjacent position to a corner of the level-generation area, 
		// or an edge of the level-generation area:
		if (ZoneIsCornerPiece(ZoneToCompareTo))
		{	
			// Get the indices of Zones that are suitable for this space:
			FindApplicableZoneIndices(ApplicableZoneIndices);
			// Then choose one of these at random:
			return GetApplicableZoneIndex(ApplicableZoneIndices);
		}

		if (ZoneIsEdgePiece(ZoneToCompareTo))
		{
			FindApplicableZoneIndices(ApplicableZoneIndices);
			return GetApplicableZoneIndex(ApplicableZoneIndices);
		}
		*/
		// The placed Zone is WangTile2 or WangTile10:
		if (ZoneIsWangTile2Or10(ZoneToCompareTo, true))
		{
			// Pick based on the adjacency of ZoneToCompareTo against a possible Zone-Index from one
			// of the pre-defined sets of indicies, for valid tiles that can be placed next to 
			// WangTile2 or WangTile10:
			return PickZoneConsideringAdjacencyToWangTile10Or2(PlacedZoneAdjacency, ZoneToCompareTo);
		}

		if (ZoneIsWangTile2Or10(ZoneToCompareTo, false))
		{
			return PickZoneConsideringAdjacencyToWangTile10Or2(PlacedZoneAdjacency, ZoneToCompareTo);
		}

		// Otherwise, call these functions to find an applicable Zone index:
		FindApplicableZoneIndices(ApplicableZoneIndices);
		return GetApplicableZoneIndex(ApplicableZoneIndices);
	}

	// No Zone found:
	return -1;
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetTargetZone(int ZoneChoice)
{
	// Return the Blueprint that represents this Zone:
	for (int ZoneBlueprintIterator = 0; ZoneBlueprintIterator < LevelZones.Num(); ZoneBlueprintIterator++)
	{
		if (ZoneSubSet[ZoneChoice]->GetName() == LevelZones[ZoneBlueprintIterator]->GetName())
		{
			PlacedLevelZones.Add(ZoneSubSet[ZoneChoice]);
			return LevelZoneTileBlueprints[ZoneBlueprintIterator];
		}
	}

	return nullptr;
}

// Helper functions:

bool UBalancedFPSLevelGeneratorTool::ZoneIsEdgePiece(int ConsideredZone)
{
	return ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_NINETEEN) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_TWENTY) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_TWENTY_ONE) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_TWENTY_TWO);
}

bool UBalancedFPSLevelGeneratorTool::ZoneIsCornerPiece(int ConsideredZone)
{
	return ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_THREE) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_FOUR) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_FIVE) ||
		ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_SIX);
}

bool UBalancedFPSLevelGeneratorTool::ZoneHasPureEvenZoneDispersion(float ConsideredZoneDispersionCoefficient)
{
	return ConsideredZoneDispersionCoefficient == PURE_EVEN_ZONE_DISPERSION;	
}

bool UBalancedFPSLevelGeneratorTool::ZoneHasHalfEvenZoneDispersion(float ConsideredZoneDispersionCoefficient)
{
	return ConsideredZoneDispersionCoefficient == HALF_EVEN_ZONE_DISPERSION;
}

bool UBalancedFPSLevelGeneratorTool::ZoneIsWangTile2Or10(int ConsideredZone, bool CheckingForWangTile2)
{
	if (CheckingForWangTile2)
	{
		return ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_TWO);
	}
	// WangTile10:
	else
	{
		return ZoneSubSet[ConsideredZone]->ActorHasTag(ZoneSubSet[ConsideredZone]->WANG_TILE_TEN);
	}	
}

// To find an applicable Zone for this space in the level-generation area:
void UBalancedFPSLevelGeneratorTool::FindApplicableZoneIndices(std::vector<int>& ApplicableZoneIndices)
{
	// Choose a Zone with a lower value than this piece's Dispersion
	// Coefficient:
	for (int ZoneIterator = 0; ZoneIterator < ZoneSubSet.Num() - 1;
		ZoneIterator++)
	{
		if (ZoneSubSet[ZoneIterator]->GetDispersonCoefficient() <
			CORNER_PIECE_ZONE_DISPERSION)
		{
			ApplicableZoneIndices.push_back(ZoneIterator);
		}
	}
}

int UBalancedFPSLevelGeneratorTool::GetApplicableZoneIndex(std::vector<int>& ApplicableZoneIndices)
{
	// To Pick from an index-subset on a random basis:
	std::default_random_engine RNG;
	std::uniform_int_distribution<int> RandomDistribution(0,
		ApplicableZoneIndices.size() - 1);
	RNG.seed(time(NULL));

	// Make a call to 'flush' the stream before using it, as well as seeding it:
	RandomDistribution(RNG);

	return ApplicableZoneIndices[RandomDistribution(RNG)];
}

int UBalancedFPSLevelGeneratorTool::PickZoneConsideringAdjacencyToWangTile10Or2(
	ZoneAdjacencyDirection PlacedZoneAdjacency, int ConsideredAdjacentZoneID)
{
	// Consider the adjacency:
	switch (PlacedZoneAdjacency)
	{
		// Same for either here (concerning the traversable direction of
		// WangTile2 or WangTile10):
	case ZoneAdjacencyDirection::Westwards:
	case ZoneAdjacencyDirection::Southwards:
		if (ConsideredAdjacentZoneID == 2)
		{
			return GetApplicableZoneIndex(ApplicableZoneIndicesForWangTile2);
		}
		// For WangTile10:
		else
		{
			return GetApplicableZoneIndex(ApplicableZoneIndicesForWangTile10);
		}
		break;

	default:
		break;
	}

	return 0;
}
