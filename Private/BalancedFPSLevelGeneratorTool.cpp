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
	
	WallPanelBlueprintAsset = ConstructorHelpers::FObjectFinder<UBlueprint>(
		TEXT("Blueprint'/Game/BalancedFPSLevelGeneratorAssets/Blueprints/WallPanel.WallPanel'"))
		.Object;

	for (int ZoneBlueprintCounter = 1; ZoneBlueprintCounter < TOTAL_ZONE_BLUEPRINT_COUNT;
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
	VariationProbability = 0.10f;
	RecombinationProbability = 0.750f;
	MutationValue = 1.0f;
	AddMutationProbability = 0.10f;
	MultiplyMutationProbability = 0.750f;

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
	// For each wall panel to use in initialisation:
	static AActor* WallPanelActor;

	// For testing:
	int FrontFaceLoopCycleCount = 0;
	int RightFaceLoopCycleCount = 0;
	int BackFaceLoopCycleCount = 0;
	int LeftFaceLoopCycleCount = 0;

	
	// Offset by DEFAULT_ENCAPSULATION_OFFSET, to allow Zones to fill in these gaps:
	for (float CurrentZPosition = 0; CurrentZPosition < DEFAULT_TILE_HEIGHT;
		CurrentZPosition += DEFAULT_TILE_HEIGHT)
	{
		// Front face:
		for (float CurrentXPosition = LevelGenerationStartPoint.X;
			CurrentXPosition < LevelExtents.X; CurrentXPosition +=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(CurrentXPosition, 
				LevelGenerationStartPoint.Y - DEFAULT_ENCAPSULATION_OFFSET, CurrentZPosition
				- DEFAULT_ENCAPSULATION_OFFSET);
			FRotator FrontFaceRotation = FRotator::ZeroRotator;

			FTransform LevelPanelTransform = FTransform(FrontFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);
			
			// *GEditor->GetEditorWorldContext().ExternalReferences[0] just to get the world context...
			WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
				WallPanelBlueprintAsset, LevelPanelTransform, false);
			WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
			FrontFaceLoopCycleCount++;
			//UGameplayStatics::FinishSpawningActor(WallPanelActor, LevelPanelTransform);
		}

		// Right face:
		for (float CurrentYPosition = LevelGenerationStartPoint.Y;
			CurrentYPosition < LevelExtents.Y; CurrentYPosition +=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(LevelGenerationStartPoint.X + LevelExtents.X + DEFAULT_ENCAPSULATION_OFFSET,
				CurrentYPosition, CurrentZPosition - DEFAULT_ENCAPSULATION_OFFSET);
			FRotator RightFaceRotation = FRotator(0.0f, 90.0f, 0.0f);

			FTransform LevelPanelTransform = FTransform(RightFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);

			// *GEditor->GetEditorWorldContext().ExternalReferences[0] just to get the world context...
			WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
				WallPanelBlueprintAsset, LevelPanelTransform, false);
			WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
			RightFaceLoopCycleCount++;
		}

		// Back face:
		for (float CurrentXPosition = LevelGenerationStartPoint.X + LevelExtents.X;
			CurrentXPosition > LevelGenerationStartPoint.X; CurrentXPosition -=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(CurrentXPosition,
				LevelGenerationStartPoint.Y + LevelExtents.Y + DEFAULT_ENCAPSULATION_OFFSET,
				CurrentZPosition - DEFAULT_ENCAPSULATION_OFFSET);
			FRotator BackFaceRotation = FRotator(0.0f, 180.0f, 0.0f);

			FTransform LevelPanelTransform = FTransform(BackFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);

			// *GEditor->GetEditorWorldContext().ExternalReferences[0] just to get the world context...
			WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
				WallPanelBlueprintAsset, LevelPanelTransform, false);
			WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
			BackFaceLoopCycleCount++;
		}

		// Left face:
		for (float CurrentYPosition = LevelGenerationStartPoint.Y + LevelExtents.Y;
			CurrentYPosition > LevelGenerationStartPoint.Y; CurrentYPosition -=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(LevelGenerationStartPoint.X - DEFAULT_ENCAPSULATION_OFFSET,
				CurrentYPosition, CurrentZPosition - DEFAULT_ENCAPSULATION_OFFSET);
			FRotator FrontFaceRotation = FRotator(0.0f, -90.0f, 0.0f);

			FTransform LevelPanelTransform = FTransform(FrontFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);

			// *GEditor->GetEditorWorldContext().ExternalReferences[0] just to get the world context...
			WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
				WallPanelBlueprintAsset, LevelPanelTransform, false);
			WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
			LeftFaceLoopCycleCount++;
		}

		// Top and bottom faces:
		for (float CurrentYPosition = LevelGenerationStartPoint.Y;
			CurrentYPosition < LevelExtents.Y; CurrentYPosition +=
			DEFAULT_TILE_WIDTH)
		{			
			for (float CurrentXPosition = LevelGenerationStartPoint.X;
				CurrentXPosition < LevelExtents.X; CurrentXPosition +=
				DEFAULT_TILE_WIDTH)
			{
				FVector CurrentPosition = FVector(CurrentXPosition,
					CurrentYPosition, CurrentZPosition - DEFAULT_ENCAPSULATION_OFFSET);
				// It seems rotation has been shuffled to the left, with a 
				// wrap around (Z for Y, Y for X etc.) in UE4:
				FRotator TopBottomFaceRotation = FRotator(0.0f, 0.0f, 90.0f);

				FTransform LevelPanelTransform = FTransform(TopBottomFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);

				// For the bottom tile first...
				WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
					WallPanelBlueprintAsset, LevelPanelTransform, false);
				WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
				// ...then the top tile:
				FVector TopTilePosition = FVector(CurrentXPosition, CurrentYPosition, CurrentZPosition + DEFAULT_TILE_HEIGHT + DEFAULT_ENCAPSULATION_OFFSET);
				LevelPanelTransform.SetComponents(LevelPanelTransform.GetRotation(), TopTilePosition, LevelPanelTransform.GetScale3D());
				WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(*GEditor->GetEditorWorldContext().ExternalReferences[0],
					WallPanelBlueprintAsset, LevelPanelTransform, false);
				WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
			}
		}
	}
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
// ADD THE POSSIBLE ZONES FOR THE EDGES TO A MANUAL LIST!
void UBalancedFPSLevelGeneratorTool::AddZonesToLevelGenerationArea()
{
	// For each Zone to use in initialisation:
	static AActor* ZoneTile;

	// Set-up the relative corner positions:
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

	// To store the colours of the Edges in each Zone:
	ZonesEdgeColours = ZoneTileEdgeColours(LevelZones.Num());

	// Set all of the edge colours in this struct to match-up with the edge colours
	// for each Edge in each Zone:
	for (int ZoneEdgeColoursCounter = 0; ZoneEdgeColoursCounter < LevelZones.Num();
		ZoneEdgeColoursCounter++)
	{
		for (int ZoneEdgeColoursIterator = 0; ZoneEdgeColoursIterator < AZone::DEFAULT_ZONE_EDGE_COUNT;
			ZoneEdgeColoursIterator++)
		{
			ZonesEdgeColours.AllEdgeColours[ZoneEdgeColoursCounter][ZoneEdgeColoursIterator] =
				LevelZones[ZoneEdgeColoursCounter]->GetZoneEdgeColours()[ZoneEdgeColoursIterator];
		}		
	}

	// The main loop to place the zones:
	for (float CurrentZPosition = LevelGenerationStartPoint.Z;
		CurrentZPosition < DEFAULT_TILE_HEIGHT; CurrentZPosition +=
		DEFAULT_TILE_HEIGHT)
	{
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
					CurrentYPosition, CurrentZPosition);
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
				// Nullify the ZoneTileBlueprint, so that it is set again:
				ZoneTileBlueprint = nullptr;

				// Sanity check:
				if (ZoneTile)
				{
					ZoneTile->ExecuteConstruction(LevelZoneTransform, nullptr, nullptr, true);
				}				

				int WHAT = 0;

				printf("%d", WHAT);				
			}
		}	
	}

	// Clear up the placed level Zones for the next level generated:
	PlacedLevelZones.Empty();
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetSuitableZoneTile(FVector2D CurrentPlacementPosition)
{
	/**
	* The Edge colours to match a Zone against, to find a suitable Zone for this CurrentPlacementPosition.
	* Zone Edge order:
	* First element: North Edge.
	* Second element: East Edge.
	* Third element: South Edge.
	* Fourth element: West Edge.
	*/
	TargetEdgeColours = 
		std::vector<FPSLevelGeneratorEdge::EdgeColour>(size_t(AZone::DEFAULT_ZONE_EDGE_COUNT));
	// For the index to find the target Zone, from the array of Zones:
	int ZoneChoice = 0;

	// Then fill it with all of the zones (these will be narrowed down to the
	// final choice for this space, later):
	ZoneSubSet = LevelZones;

	// If this Zone is to be placed against an edge of
	// the level generation area, then the tile will 
	// have to match 'pure' wall 'edges', as well as
	// the Edge colour of any other adjacent Zones:

	// South level-generation area 'edge':
	if (CurrentPlacementPosition.Y == LevelGenerationStartPoint.Y)
	{
		TargetEdgeColours[2] = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	// North level-generation area 'edge':
	if (CurrentPlacementPosition.Y == LevelExtents.Y)
	{
		TargetEdgeColours[0] = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	// West level-generation area 'edge':
	if (CurrentPlacementPosition.X == LevelGenerationStartPoint.X)
	{
		TargetEdgeColours[3] = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	// East level-generation area 'edge':
	if (CurrentPlacementPosition.X == LevelExtents.X)
	{
		TargetEdgeColours[1] = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	// Check to see if the function can return a value here:
	
	// For choosing from a particular set of Zones:
	std::default_random_engine RNG;
	std::uniform_int_distribution<int> RandomDistribution;
	RNG.seed(time(NULL));
	
	// Only set to true if one of the below conditions is true:
	bool PlacementInCornerOrAlongEdge = false;

	// ADD MORE COMBINATIONS!! GWAGIU*WA
	// RESOLVE ISSUES WITH PLACEMENT OF ZONES IN THE CORNERS
	// OF THE LEVEL GENERATION AREA!@GAWI

	// Choose from Zones 1, 3 or 14:
	if (CurrentPlacementPosition == TopLeftCorner)
	{
		RandomDistribution = std::uniform_int_distribution<int>(0, 2);
		RNG.seed(time(NULL));
		int RandomChoice = RandomDistribution(RNG);
		std::vector<int> ZoneIndices = { ZONE_ONE_INDEX, ZONE_THREE_INDEX,
			ZONE_FOURTEEN_INDEX };
		
		ZoneChoice = ZoneIndices[RandomChoice];

		PlacementInCornerOrAlongEdge = true;
	}
	// Choose from Zones 4, 14 or 15:
	else if (CurrentPlacementPosition == TopRightCorner)
	{
		RandomDistribution = std::uniform_int_distribution<int>(0, 2);
		RNG.seed(time(NULL));
		int RandomChoice = RandomDistribution(RNG);
		std::vector<int> ZoneIndices = { ZONE_FOUR_INDEX, ZONE_FOURTEEN_INDEX,
			ZONE_FIFTHTEEN_INDEX };

		ZoneChoice = ZoneIndices[RandomChoice];

		PlacementInCornerOrAlongEdge = true;
	}
	// Choose from Zones 5, 15 or 16:
	else if (CurrentPlacementPosition == BottomRightCorner)
	{
		RandomDistribution = std::uniform_int_distribution<int>(0, 2);
		RNG.seed(time(NULL));
		int RandomChoice = RandomDistribution(RNG);
		std::vector<int> ZoneIndices = { ZONE_FIVE_INDEX, ZONE_FIFTHTEEN_INDEX,
			ZONE_SIXTEEN_INDEX };

		ZoneChoice = ZoneIndices[RandomChoice];

		PlacementInCornerOrAlongEdge = true;
	}
	// Choose from Zones 1, 6 or 16:
	else if (CurrentPlacementPosition == BottomLeftCorner)
	{
		RandomDistribution = std::uniform_int_distribution<int>(0, 2);
		RNG.seed(time(NULL));
		int RandomChoice = RandomDistribution(RNG);
		std::vector<int> ZoneIndices = { ZONE_ONE_INDEX, ZONE_SIX_INDEX,
			ZONE_SIXTEEN_INDEX };

		ZoneChoice = ZoneIndices[RandomChoice];

		PlacementInCornerOrAlongEdge = true;
	}

	// A Zone will be placed in a corner or along an Edge of the level-generation area:
	if (PlacementInCornerOrAlongEdge)
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
	}

	// Check through PlacedLevelZones, to complete TargetEdgeColours.
	// (If any Zones have been placed):
	for (int PlacedZonesIterator = 0; PlacedZonesIterator < PlacedLevelZones.Num();
		PlacedZonesIterator++)
	{
		if (PlacedLevelZones.Num() == 0)
		{
			// No zones have been placed:
			break;
		}

		// Get the Edge colours of this Zone:
		std::vector<FPSLevelGeneratorEdge::EdgeColour> PlacedEdgeColours = PlacedLevelZones[PlacedZonesIterator]->GetZoneEdgeColours();

		// There is a tile to the west:
		if (PlacedLevelZones[PlacedZonesIterator]->GetActorLocation().X ==
			(CurrentPlacementPosition.X - DEFAULT_TILE_WIDTH))
		{
			TargetEdgeColours[3] = PlacedEdgeColours[3];
		}

		// There is a tile to the south:
		if (PlacedLevelZones[PlacedZonesIterator]->GetActorLocation().Y ==
			(CurrentPlacementPosition.Y - DEFAULT_TILE_HEIGHT))
		{
			TargetEdgeColours[2] = PlacedEdgeColours[2];
		}
	}

	/** 
	Not nessessary in the latest version, this class will instead determine which Zone to use, based on how Edges
	match up against each other
	

	// Get a set of Zones that match the TargetEdgeColours...
	for (int ZoneSetIterator = 0; ZoneSetIterator < LevelZoneTiles.Num(); ZoneSetIterator++)
	{
		// For the Edge-colours of this current Zone in the set:
		std::vector<FPSLevelGeneratorEdge::EdgeColour> CurrentZoneEdgeColours = LevelZoneTiles[ZoneSetIterator]->GetZoneEdgeColours();

		// For validating if a tile matches up to the TargetEdgeColours:
		bool ZoneMatchesToTargetEdgeColours = true;

		// To check each EdgeColour against CurrentZoneEdgeColours:
		for (int TargetEdgeColoursIterator = 0; TargetEdgeColoursIterator < TargetEdgeColours.size() - 1;
			TargetEdgeColoursIterator++)
		{
			if (TargetEdgeColours[TargetEdgeColoursIterator] > FPSLevelGeneratorEdge::EdgeColour::Grey - 1)
			{
				// This CurrentEdgeColour would not match up to TargetEdgeColour, so this zone would not go
				// in the CurrentPlacementPosition.
				// For the mean time, grey is assumed to be colourless (so matching with any colour):
				if (!(CurrentZoneEdgeColours[TargetEdgeColoursIterator] == TargetEdgeColours[TargetEdgeColoursIterator]) &&
					TargetEdgeColours[TargetEdgeColoursIterator] != FPSLevelGeneratorEdge::EdgeColour::Grey)
				{
					ZoneMatchesToTargetEdgeColours = false;
					break;
				}
			}
		}

		if (ZoneMatchesToTargetEdgeColours)
		{
			ZoneSubSet.Add(LevelZoneTiles[ZoneSetIterator]);
		}
	}
	
	// ...then pick one of these Zones from the sub-set:

	// But if there are no Zones in the sub-set:
	if (ZoneSubSet.Num() == 0)
	{
		return nullptr;
	}
	*/

	ZoneChoice = GetZoneChoiceIndex();
	
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

/**
	RESOLVE ISSUES WITH THIS FUNCTION'S DETERMINATION OF MATCHING EDGE TILES!!!IFAH*_FPYAW
*/
int UBalancedFPSLevelGeneratorTool::GetZoneChoiceIndex()
{
	// The return value:
	int NewZoneChoice = -1;

	// For which edge will match up to an edge, depending on the colour:
	int RNGResult = 0;

	// To determine the chance of a Zone with an Edge of a certain colour,
	// getting chosen:	
	std::default_random_engine RNG;
	std::uniform_int_distribution<int> RandomDistribution(0, 100);

	// Seed the RNG before using it:
	RNG.seed(time(NULL));

	// Make a call to 'flush' the stream before using it, as well as seeding it:
	RandomDistribution(RNG);

	// For the Edge colours that match against TargetEdgeColours (to find the Zone that has those Edge colours):
	std::vector<FPSLevelGeneratorEdge::EdgeColour> MatchingEdgeColours =
		std::vector<FPSLevelGeneratorEdge::EdgeColour>(AZone::DEFAULT_ZONE_EDGE_COUNT);

	/**
	* For tracking which Edge to attempt to 'match' against.
	* Start at the west Edge and move through the Edges anti-
	* clockwise.
	* Zone Edge order:
	* First element: North Edge.
	* Second element: East Edge.
	* Third element: South Edge.
	* Fourth element: West Edge.
	*/
	for (int CurrentEdgePosition = TargetEdgeColours.size() - 1; CurrentEdgePosition > 0;
		CurrentEdgePosition--)
	{		
		// Match against colourless:
		if (TargetEdgeColours[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Colourless)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstColourless(RNGResult);
		}
		// Match against blue:
		else if (TargetEdgeColours[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Blue)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstBlue(RNGResult);
		}
		// Match against green:
		else if (TargetEdgeColours[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Green)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstGreen(RNGResult);
		}
		// Match against red:
		else if (TargetEdgeColours[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Red)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstRed(RNGResult);
		}
		// Match against grey:
		else if (TargetEdgeColours[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Grey)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstGrey(RNGResult);
		}
	}

	// Make sure a colourless edge has not been chosen (as there are no tiles with a colourless edge):
	for (int MatchingEdgeColoursIterator = 0; MatchingEdgeColoursIterator < MatchingEdgeColours.size() - 1;
		MatchingEdgeColoursIterator++)
	{
		if (MatchingEdgeColours[MatchingEdgeColoursIterator] == FPSLevelGeneratorEdge::EdgeColour::Colourless)
		{
			RNGResult = RandomDistribution(RNG);
			MatchingEdgeColours[MatchingEdgeColoursIterator] = MatchAgainstColourless(RNGResult);
		}
	}

	// Check for a zone that matches these colours,
	// or if there is no absolute match, Find the closest match:
	bool AllEdgesMatch = true;
	int MatchingEdgeCount = 0;
	
	// For the indices of the edges to be considered,
	// if no absolute match has been found:
	std::vector<int> ConsideredEdgeIndices;
	// The indices of these vectors will match-up,
	// so the edge at a certain index will have a 
	// count of edges that match to the MatchingEdgeColours:
	std::vector<int> ConsideredEdgeCount;

	for (int ZoneTileEdgeColoursIterator = 0; ZoneTileEdgeColoursIterator < ZonesEdgeColours.AllEdgeColours.size() - 1;
		ZoneTileEdgeColoursIterator++)
	{
		MatchingEdgeCount = 0;

		for (int ZoneEdgeColoursIterator = 0; ZoneEdgeColoursIterator < AZone::DEFAULT_ZONE_EDGE_COUNT;
			ZoneEdgeColoursIterator++)
		{
			if (ZonesEdgeColours.AllEdgeColours[ZoneTileEdgeColoursIterator][ZoneEdgeColoursIterator]
				!= MatchingEdgeColours[ZoneEdgeColoursIterator])
			{
				AllEdgesMatch = false;
			}
			else if (ZonesEdgeColours.AllEdgeColours[ZoneTileEdgeColoursIterator][ZoneEdgeColoursIterator]
				== MatchingEdgeColours[ZoneEdgeColoursIterator])
			{
				MatchingEdgeCount++;
			}			
		}	

		if (AllEdgesMatch)
		{
			NewZoneChoice = ZoneTileEdgeColoursIterator;
			break;
		}
		else if (MatchingEdgeCount >= INCOMPLETE_MATCH_THRESHOLD)
		{
			ConsideredEdgeIndices.push_back(ZoneTileEdgeColoursIterator);
			ConsideredEdgeCount.push_back(MatchingEdgeCount);
		}
	}

	// No absolute match has been found,
	// so find the next best value from
	// ConsideredEdges:
	
	if (NewZoneChoice == -1)
	{	
		//ISSUE HERE SEEMS TO BE THE USE OF AUTO OR AN ITERATOR,
		//RESOLVE THISG(£UAGUA"(G"UA(
		// Only for the indices:
		std::vector<int> ClosestMatchingEdges;
		int HighestMatchingEdgeCount = 0;

		// Get the highest number of matching edges...
		for (int ConsideredEdgeCountIterator = 0; ConsideredEdgeCountIterator <
			ConsideredEdgeCount.size() - 1; ConsideredEdgeCountIterator++)
		{
			if (HighestMatchingEdgeCount < ConsideredEdgeCount[ConsideredEdgeCountIterator])
			{
				HighestMatchingEdgeCount = ConsideredEdgeCount[ConsideredEdgeCountIterator];
			}			
		}

		// ...then add the index of each edge to ClosestMatchingEdges:
		for (int ConsideredEdgeCountIterator = 0; ConsideredEdgeCountIterator <
			ConsideredEdgeIndices.size() - 1; ConsideredEdgeCountIterator++)
		{
			if (ConsideredEdgeCount[ConsideredEdgeCountIterator] == HighestMatchingEdgeCount)
			{
				ClosestMatchingEdges.push_back(ConsideredEdgeIndices[ConsideredEdgeCountIterator]);
			}			
		}

		// Use RandomDistribution and RNG to choose from ClosestMatchingEdges:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ClosestMatchingEdges.size() - 1);
		RNG.seed(time(NULL));
		RandomDistribution(RNG);

		NewZoneChoice = ClosestMatchingEdges[RandomDistribution(RNG)];
	}
	
	return NewZoneChoice;
}

// For matching against certain Edges:
FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstColourless(int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult = FPSLevelGeneratorEdge::EdgeColour::Colourless;

	// Put the constant values into a vector to sort them by descending order
	// (to compare them against the result, no matter the values of these constant members):
	std::vector<int> ColourlessRangeValues = { COLOURLESS_TO_BLUE, COLOURLESS_TO_GREEN, COLOURLESS_TO_GREY,
	COLOURLESS_TO_RED };

	// Red edge:
	if (RNGResult < ColourlessRangeValues[3])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Grey edge:
	else if (RNGResult >= ColourlessRangeValues[3] &&
		RNGResult < ColourlessRangeValues[3] + ColourlessRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}
	// Green edge:
	else if (RNGResult >= ColourlessRangeValues[3] + ColourlessRangeValues[2] &&
		RNGResult < ColourlessRangeValues[3] + ColourlessRangeValues[2] + ColourlessRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Blue edge:
	else if (RNGResult >= ColourlessRangeValues[3] + ColourlessRangeValues[2] +
		ColourlessRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstGrey(int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult = FPSLevelGeneratorEdge::EdgeColour::Colourless;

	// Put the constant values into a vector to sort them by descending order
	// (to compare them against the result, no matter the values of these constant members):
	std::vector<int> GreyRangeValues = { GREY_TO_BLUE, GREY_TO_GREEN, GREY_TO_GREY,
		GREY_TO_RED };

	// Red edge:
	if (RNGResult < GreyRangeValues[0])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Grey edge:
	else if (RNGResult >= GreyRangeValues[0] &&
		RNGResult < GreyRangeValues[0] + GreyRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}
	// Green edge:
	else if (RNGResult >= GreyRangeValues[0] + GreyRangeValues[1] &&
		RNGResult < GreyRangeValues[0] + GreyRangeValues[1] + GreyRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Blue edge:
	else if (RNGResult >= GreyRangeValues[0] + GreyRangeValues[1] +
		GreyRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstRed(int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult = FPSLevelGeneratorEdge::EdgeColour::Colourless;

	// Put the constant values into a vector to sort them by descending order
	// (to compare them against the result, no matter the values of these constant members):
	std::vector<int> RedRangeValues = { RED_TO_BLUE, RED_TO_GREEN, RED_TO_GREY,
		RED_TO_RED };

	// Red edge:
	if (RNGResult < RedRangeValues[0])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Grey edge:
	else if (RNGResult >= RedRangeValues[0] &&
		RNGResult < RedRangeValues[0] + RedRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}
	// Green edge:
	else if (RNGResult >= RedRangeValues[0] + RedRangeValues[1] &&
		RNGResult < RedRangeValues[0] + RedRangeValues[1] + RedRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Blue edge:
	else if (RNGResult >= RedRangeValues[0] + RedRangeValues[1] +
		RedRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstGreen(int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult = FPSLevelGeneratorEdge::EdgeColour::Colourless;

	// Put the constant values into a vector to sort them by descending order
	// (to compare them against the result, no matter the values of these constant members):
	std::vector<int> GreenRangeValues = { GREEN_TO_BLUE, GREEN_TO_GREEN, GREEN_TO_GREY,
		GREEN_TO_RED };

	// Red edge:
	if (RNGResult < GreenRangeValues[0])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Grey edge:
	else if (RNGResult >= GreenRangeValues[0] &&
		RNGResult < GreenRangeValues[0] + GreenRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}
	// Green edge:
	else if (RNGResult >= GreenRangeValues[0] + GreenRangeValues[1] &&
		RNGResult < GreenRangeValues[0] + GreenRangeValues[1] + GreenRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Blue edge:
	else if (RNGResult >= GreenRangeValues[0] + GreenRangeValues[1] +
		GreenRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstBlue(int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult = FPSLevelGeneratorEdge::EdgeColour::Colourless;

	// Put the constant values into a vector to sort them by descending order
	// (to compare them against the result, no matter the values of these constant members):
	std::vector<int> BlueRangeValues = { BLUE_TO_BLUE, BLUE_TO_GREEN, BLUE_TO_GREY,
		BLUE_TO_RED };

	// Blue edge:
	if (RNGResult < BlueRangeValues[0])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}
	// Red edge:
	else if (RNGResult >= BlueRangeValues[0] &&
		RNGResult < BlueRangeValues[0] + BlueRangeValues[1])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Green edge:
	else if (RNGResult >= BlueRangeValues[0] + BlueRangeValues[1] &&
		RNGResult < BlueRangeValues[0] + BlueRangeValues[1] + BlueRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Grey edge:
	else if (RNGResult >= BlueRangeValues[0] + BlueRangeValues[1] +
		BlueRangeValues[2])
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}

	return ColourResult;
}
