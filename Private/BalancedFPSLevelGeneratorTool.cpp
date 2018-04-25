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

// Initialise:
UBalancedFPSLevelGeneratorTool::UBalancedFPSLevelGeneratorTool()
{
	// For non-dynamic setting of applicable Zones, for Wang Tile 2 and 10:
	ApplicableZoneIndicesForWangTile2 = { 3, 4, 6, 7, 9, 10, 11, 14 };
	ApplicableZoneIndicesForWangTile10 = { 4, 5, 6, 11, 12, 15, 16, 18 };

	// Starting at 0:
	CurrentPlacedZonePositionsKeyValue = 0;

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

			// RESOLVE ISSUES WITH THE PLACEMENT OF ZONES THAT ARE NOT CORNER OR EDGE PIECES: 

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
	PlacedZonePositions.empty();
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetSuitableZoneTile(FVector2D CurrentPlacementPosition)
{	
	// For the index to find the target Zone, from the array of Zones:
	int ZoneChoice = -1;

	// Flags:

	bool PlacementInCorner = false;
	bool PlacementAlongEdge = false;	

	// Check to see if the function can return a value here,
	// before checking against the placed Zones in the level
	// -generation area:

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
		LevelZones[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(3.0f, 2.0f);
		AddValueToCurrentPlacedZonePositions(CurrentPlacementPosition);
		return GetTargetZone(ZoneChoice);
	}

	if (PlacementAlongEdge)
	{
		LevelZones[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(5.0f, 3.0f);
		AddValueToCurrentPlacedZonePositions(CurrentPlacementPosition);
		return GetTargetZone(ZoneChoice);
	}

	// To not have to check this in the following for loop:
	ZoneToSouthAndOrWestCheck ZoneToSouthAndOrWestCheckInstance = ZoneToSouthAndOrWest(CurrentPlacementPosition);

	// Check If any Zones have been placed:
	for (int PlacedZonesIterator = 0; PlacedZonesIterator < PlacedLevelZones.Num();
		PlacedZonesIterator++)
	{
		if (PlacedLevelZones.Num() == 0)
		{
			// No zones have been placed:
			break;
		}

		// There is a Zone to the south and the west:
		if (ZoneToSouthAndOrWestCheckInstance.IsZoneToSouth && ZoneToSouthAndOrWestCheckInstance.IsZoneToWest)
		{
			ZoneChoice = GetZoneConsideringCoefficientsForZonesSouthAndWestOfPosition(ZoneToSouthAndOrWestCheckInstance.ZoneToSouthAndWestIndices);
		}

		// Only a Zone to the south:
		if (ZoneToSouthAndOrWestCheckInstance.IsZoneToSouth)
		{
			ZoneChoice = GetZoneConsideringCoefficients(PlacedZonesIterator);
		}
		// Only a Zone to the west:
		else if (ZoneToSouthAndOrWestCheckInstance.IsZoneToWest)
		{
			ZoneChoice = GetZoneConsideringCoefficients(PlacedZonesIterator);
		}
		
		if (ZoneChoice != -1)
		{
			// For a Zone that will be placed in a position that is not in a corner, or along an
			// edge of the level-generation area:
			LevelZones[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(8.0f, 4.0f);
			AddValueToCurrentPlacedZonePositions(CurrentPlacementPosition);
			return GetTargetZone(ZoneChoice);
		}
	}

	//if (ZoneChoice == -1)
	//{
	//	// Test for now:
	//	ZoneChoice = GetZoneConsideringCoefficients(0,
	//			ZoneAdjacencyDirection::Westwards);	
	//	LevelZones[ZoneChoice]->DetermineDefensivenessAndFlankingCoefficients(8.0f, 4.0f);
	//	return GetTargetZone(ZoneChoice);
	//}

	// Flow should never reach this point:

	return nullptr;
}

// The coefficients will be considered here, for the choice of Zone to place (not the Edge-Colour):
int UBalancedFPSLevelGeneratorTool::GetZoneConsideringCoefficients(int ZoneToCompareTo)
{
	// For the Coefficients to consider:
	float ConsideredZoneDefensivenessCoefficient = PlacedLevelZones[ZoneToCompareTo]->GetDefensivenessCoefficient();
	float ConsideredZoneDispersionCoefficient = PlacedLevelZones[ZoneToCompareTo]->GetDispersonCoefficient();

	// Check through all of the Zones to find a suitable Zone for placement:
	for (int ZoneIterator = 0; ZoneIterator < LevelZones.Num() - 1;
		ZoneIterator++)
	{
		// The placed Zone is WangTile2 or WangTile10:
		if (ZoneIsWangTile2Or10(ZoneToCompareTo, true))
		{
			// Pick based on the adjacency of ZoneToCompareTo against a possible Zone-Index from one
			// of the pre-defined sets of indicies, for valid tiles that can be placed next to 
			// WangTile2 or WangTile10:
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZoneToCompareTo);
		}
		else if (ZoneIsWangTile2Or10(ZoneToCompareTo, false))
		{
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZoneToCompareTo);
		}
		// Considering Defensiveness:
		else if (ConsideredZoneDefensivenessCoefficient >= ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD)
		{
			FindApplicableZoneIndicesConsideringDefensiveness(true);
		}
		else if (ConsideredZoneDefensivenessCoefficient <= ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD)
		{
			FindApplicableZoneIndicesConsideringDefensiveness(false);
		}
		else
		{
			// Otherwise, find applicable Zone indices based on Dispersion:
			FindApplicableZoneIndicesConsideringDispersion(ConsideredZoneDispersionCoefficient);
		}
			
		return GetApplicableZoneIndex(ZoneCollectionToChoose::OtherCollection);
	}

	// No Zone found:
	return -1;
}

// Handle placement of a Zone, which will have 4 adjacent Zones:
int UBalancedFPSLevelGeneratorTool::GetZoneConsideringCoefficientsForZonesSouthAndWestOfPosition(int ZonesToCompareTo[2])
{
	// For the Coefficients to consider (taking the mean-average of both values):
	float MeanConsideredZoneDefensivenessCoefficient = (PlacedLevelZones[ZonesToCompareTo[0]]->GetDefensivenessCoefficient() +
		PlacedLevelZones[ZonesToCompareTo[1]]->GetDefensivenessCoefficient()) / 2;
	float MeanConsideredZoneDispersionCoefficient = (PlacedLevelZones[ZonesToCompareTo[0]]->GetDispersonCoefficient() +
		PlacedLevelZones[ZonesToCompareTo[1]]->GetDispersonCoefficient()) / 2;

	// Check through all of the Zones to find a suitable Zone for placement:
	for (int ZoneIterator = 0; ZoneIterator < LevelZones.Num() - 1;
		ZoneIterator++)
	{
		// The placed Zone is WangTile2 or WangTile10:
		if (ZoneIsWangTile2Or10(ZonesToCompareTo[0], true))
		{
			// Pick based on the adjacency of ZoneToCompareTo against a possible Zone-Index from one
			// of the pre-defined sets of indicies, for valid tiles that can be placed next to 
			// WangTile2 or WangTile10:
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZonesToCompareTo[0]);
		}
		else if (ZoneIsWangTile2Or10(ZonesToCompareTo[0], false))
		{
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZonesToCompareTo[0]);
		}
		else if (ZoneIsWangTile2Or10(ZonesToCompareTo[1], true))
		{
			// Pick based on the adjacency of ZoneToCompareTo against a possible Zone-Index from one
			// of the pre-defined sets of indicies, for valid tiles that can be placed next to 
			// WangTile2 or WangTile10:
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZonesToCompareTo[1]);
		}
		else if (ZoneIsWangTile2Or10(ZonesToCompareTo[1], false))
		{
			return PickZoneConsideringAdjacencyToWangTile10Or2(ZonesToCompareTo[1]);
		}
		// Considering Defensiveness:
		else if (MeanConsideredZoneDefensivenessCoefficient >= ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD)
		{
			FindApplicableZoneIndicesConsideringDefensiveness(true);
		}
		else if (MeanConsideredZoneDefensivenessCoefficient <= ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD)
		{
			FindApplicableZoneIndicesConsideringDefensiveness(false);
		}
		else
		{
			// Otherwise, find applicable Zone indices based on Dispersion:
			FindApplicableZoneIndicesConsideringDispersion(MeanConsideredZoneDispersionCoefficient);
		}

		return GetApplicableZoneIndex(ZoneCollectionToChoose::OtherCollection);
	}

	// No Zone found:
	return -1;
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetTargetZone(int ZoneChoice)
{
	// Return the Blueprint that represents this Zone:
	for (int ZoneBlueprintIterator = 0; ZoneBlueprintIterator < LevelZones.Num(); ZoneBlueprintIterator++)
	{
		if (LevelZones[ZoneChoice]->GetName() == LevelZones[ZoneBlueprintIterator]->GetName())
		{
			PlacedLevelZones.Add(LevelZones[ZoneChoice]);
			return LevelZoneTileBlueprints[ZoneBlueprintIterator];
		}
	}

	return nullptr;
}

// Helper functions:

bool UBalancedFPSLevelGeneratorTool::ZoneIsEdgePiece(int ConsideredZone)
{
	return LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_NINETEEN) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_TWENTY) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_TWENTY_ONE) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_TWENTY_TWO);
}

bool UBalancedFPSLevelGeneratorTool::ZoneIsCornerPiece(int ConsideredZone)
{
	return LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_THREE) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_FOUR) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_FIVE) ||
		LevelZones[ConsideredZone]->ActorHasTag(LevelZones[ConsideredZone]->WANG_TILE_SIX);
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
	// For any index of LevelZones, to get a constant value to identify WangTile2 or WangTile10:
	int DefaultLevelZonesIndex = 0;

	if (CheckingForWangTile2)
	{
		return PlacedLevelZones[ConsideredZone]->ActorHasTag(LevelZones[DefaultLevelZonesIndex]->WANG_TILE_TWO);
	}
	// WangTile10:
	else
	{
		return PlacedLevelZones[ConsideredZone]->ActorHasTag(LevelZones[DefaultLevelZonesIndex]->WANG_TILE_TEN);
	}	
}

// To find an applicable Zone for this space in the level-generation area:
void UBalancedFPSLevelGeneratorTool::FindApplicableZoneIndicesConsideringDispersion(float DispersionCoefficient)
{
	// Choose a Zone with a lower value than this piece's Dispersion
	// Coefficient:
	for (int ZoneIterator = 0; ZoneIterator < LevelZones.Num() - 1;
		ZoneIterator++)
	{
		if (LevelZones[ZoneIterator]->GetDispersonCoefficient() <
			DispersionCoefficient)
		{
			ApplicableZoneIndices.push_back(ZoneIterator);
		}
	}
}

int UBalancedFPSLevelGeneratorTool::GetApplicableZoneIndex(ZoneCollectionToChoose CollectionToConsider)
{
	// To Pick from the index-subset on a random basis:
	std::default_random_engine RNG;
	std::uniform_int_distribution<int> RandomDistribution;
	RNG.seed(time(NULL));

	// Make a call to 'flush' the stream before using it, as well as seeding it:
	RandomDistribution(RNG);

	// Pick from the respective collection (depending on which adjacent Zone is being considered):
	switch (CollectionToConsider)
	{
	case ZoneCollectionToChoose::WangTile2Collection:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ApplicableZoneIndicesForWangTile2.size() - 1);
		return ApplicableZoneIndicesForWangTile2[RandomDistribution(RNG)];
		break;

	case ZoneCollectionToChoose::Wangtile10Collection:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ApplicableZoneIndicesForWangTile10.size() - 1);
		return ApplicableZoneIndicesForWangTile10[RandomDistribution(RNG)];
		break;
	
	// For the ApplicableZoneIndices collection:
	case ZoneCollectionToChoose::OtherCollection:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ApplicableZoneIndices.size() - 1);
		// APPLICABLE ZONES INDICES SOMETIMES HAS NO ITEMS, RESOLVE THIS!
		return ApplicableZoneIndices[RandomDistribution(RNG)];
		break;
	}
	
	return 0;
}

int UBalancedFPSLevelGeneratorTool::PickZoneConsideringAdjacencyToWangTile10Or2(int ConsideredAdjacentZoneID)
{
	if (ConsideredAdjacentZoneID == 2)
	{
		return GetApplicableZoneIndex(ZoneCollectionToChoose::WangTile2Collection);
	}
	// For WangTile10:
	else
	{
		return GetApplicableZoneIndex(ZoneCollectionToChoose::Wangtile10Collection);
	}

	return 0;
}

void UBalancedFPSLevelGeneratorTool::FindApplicableZoneIndicesConsideringDefensiveness(bool IsGreaterThanThreshold)
{
	// Choose a Zone with a lower or greater value than the considered piece's
	// Defensiveness Coefficient:
	for (int ZoneIterator = 0; ZoneIterator < LevelZones.Num() - 1;
		ZoneIterator++)
	{
		// If the considered piece's Defensiveness is greater than or equal to the
		// threshold, find a piece with a Defensiveness less than or equal to the
		// threshold and vice versa:
		if (IsGreaterThanThreshold)
		{
			if (ZoneSubsetDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(
				ZoneIterator, false))
			{
				ApplicableZoneIndices.push_back(ZoneIterator);
			}

		}
		// Less than or equal to the threshold:
		else
		{
			if (ZoneSubsetDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(
				ZoneIterator, true))
			{
				ApplicableZoneIndices.push_back(ZoneIterator);
			}
		}
	}
}

// Boolean conditional checks:

bool UBalancedFPSLevelGeneratorTool::PlacedZoneDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(int ZoneIndexToCheckAgainstThreshold, bool IsGreaterThanOrEqualToCheck)
{
	if (IsGreaterThanOrEqualToCheck)
	{
		return PlacedLevelZones[ZoneIndexToCheckAgainstThreshold]->GetDefensivenessCoefficient() >=
			ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD;
	}
	// Less than or equal to check:
	else
	{
		return PlacedLevelZones[ZoneIndexToCheckAgainstThreshold]->GetDefensivenessCoefficient() <=
			ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD;
	}
	
	return false;
}

bool UBalancedFPSLevelGeneratorTool::ZoneSubsetDefensivenessIsGreaterThanOrEqualToOrLessThanOrEqualToThreshold(int ZoneIndexToCheckAgainstThreshold, bool IsGreaterThanOrEqualToCheck)
{
	if (IsGreaterThanOrEqualToCheck)
	{
		return LevelZones[ZoneIndexToCheckAgainstThreshold]->GetDefensivenessCoefficient() >=
			ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD;
	}
	// Less than or equal to check:
	else
	{
		return LevelZones[ZoneIndexToCheckAgainstThreshold]->GetDefensivenessCoefficient() <=
			ZONE_DEFENSIVENESS_COEFFICIENT_THRESHOLD;
	}
	
	return false;
}

UBalancedFPSLevelGeneratorTool::ZoneToSouthAndOrWestCheck UBalancedFPSLevelGeneratorTool::ZoneToSouthAndOrWest(FVector2D CurrentPlacementPosition)
{
	// The return value (with values set in the for loop):
	ZoneToSouthAndOrWestCheck ReturnCheckStructure = ZoneToSouthAndOrWestCheck();

	for (int PlacedZonesIterator = 0; PlacedZonesIterator < PlacedLevelZones.Num();
		PlacedZonesIterator++)
	{
		// There is a Zone to the south:
		if (PlacedZonePositions[PlacedZonesIterator].Y ==
			(CurrentPlacementPosition.Y - DEFAULT_TILE_HEIGHT))
		{
			ReturnCheckStructure.IsZoneToSouth = true;
			// First item in this array is for south, with the second item being for 
			// west of the current placement position:
			ReturnCheckStructure.ZoneToSouthAndWestIndices[0] = PlacedZonesIterator;
		}

		// There is a Zone to the west:
		if (PlacedZonePositions[PlacedZonesIterator].X ==
			(CurrentPlacementPosition.X - DEFAULT_TILE_WIDTH))
		{
			ReturnCheckStructure.IsZoneToWest = true;
			ReturnCheckStructure.ZoneToSouthAndWestIndices[1] = PlacedZonesIterator;
		}
	}

	// GIVEN THE ORDER OF CHECKING ZONES IsZoneToSouthAndWest IS USALLY FALSE, AS ONLY ONE OF THE FLAGS HERE
	// IS TRUE! RESOLVE THIS GPAG({"A
	return ReturnCheckStructure;
}

// For adding a value to this map, and incrementing the key of the map, to track the values:
void UBalancedFPSLevelGeneratorTool::AddValueToCurrentPlacedZonePositions(FVector2D PositionValue)
{	
	// Insert returns a pair of the instance pair's key and value, along with a bool, which will be false if the map
	// already contains an element with the same key:
	if (PlacedZonePositions.insert(std::pair<int, FVector2D>(CurrentPlacedZonePositionsKeyValue, PositionValue)).second)
	{
		CurrentPlacedZonePositionsKeyValue++;
	}	
}
