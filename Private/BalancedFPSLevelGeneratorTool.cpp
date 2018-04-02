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
#include <map>


// Initialise:
UBalancedFPSLevelGeneratorTool::UBalancedFPSLevelGeneratorTool()
{
	
	WallPanelBlueprintAsset = ConstructorHelpers::FObjectFinder<UBlueprint>(
		TEXT("Blueprint'/Game/BalancedFPSLevelGeneratorAssets/Blueprints/WallPanel.WallPanel'"))
		.Object;

	for (int ZoneBlueprintCounter = 0; ZoneBlueprintCounter < TOTAL_ZONE_BLUEPRINT_COUNT;
		ZoneBlueprintCounter++)
	{
		FString IncrementalPathString = "Blueprint'/Game/BalancedFPSLevelGeneratorAssets/Blueprints/WangTiles/";
		// As ZoneBlueprintCounter starts at 0, add one to it, when building this FString:
		IncrementalPathString.Append(FString("WangTile") + FString::FromInt(ZoneBlueprintCounter + 1) + FString(".WangTile") +
			FString::FromInt(ZoneBlueprintCounter + 1) + FString("'"));
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

	
	// Offset by DEFAULT_TILE_DEPTH, to allow Zones to fill in these gaps:
	for (float CurrentZPosition = 0; CurrentZPosition < DEFAULT_TILE_HEIGHT;
		CurrentZPosition += DEFAULT_TILE_HEIGHT)
	{
		// Front face:
		for (float CurrentXPosition = LevelGenerationStartPoint.X;
			CurrentXPosition < LevelExtents.X; CurrentXPosition +=
			DEFAULT_TILE_WIDTH)
		{
			FVector CurrentPosition = FVector(CurrentXPosition, 
				LevelGenerationStartPoint.Y - DEFAULT_TILE_DEPTH, CurrentZPosition
				- DEFAULT_TILE_DEPTH);
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
			FVector CurrentPosition = FVector(LevelGenerationStartPoint.X + LevelExtents.X + DEFAULT_TILE_DEPTH,
				CurrentYPosition, CurrentZPosition - DEFAULT_TILE_DEPTH);
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
				LevelGenerationStartPoint.Y + LevelExtents.Y + DEFAULT_TILE_DEPTH,
				CurrentZPosition - DEFAULT_TILE_DEPTH);
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
			FVector CurrentPosition = FVector(LevelGenerationStartPoint.X - DEFAULT_TILE_DEPTH,
				CurrentYPosition, CurrentZPosition - DEFAULT_TILE_DEPTH);
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
					CurrentYPosition, CurrentZPosition - DEFAULT_TILE_DEPTH);
				// It seems rotation has been shuffled to the left, with a 
				// wrap around (Z for Y, Y for X etc.) in UE4:
				FRotator TopBottomFaceRotation = FRotator(0.0f, 0.0f, 90.0f);

				FTransform LevelPanelTransform = FTransform(TopBottomFaceRotation.Quaternion(), CurrentPosition, DefaultRelativePanelScale);

				// For the bottom tile first...
				WallPanelActor = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
					WallPanelBlueprintAsset, LevelPanelTransform, false);
				WallPanelActor->ExecuteConstruction(LevelPanelTransform, nullptr, nullptr, true);
				// ...then the top tile:
				FVector TopTilePosition = FVector(CurrentXPosition, CurrentYPosition, CurrentZPosition + DEFAULT_TILE_HEIGHT + DEFAULT_TILE_DEPTH);
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
void UBalancedFPSLevelGeneratorTool::AddZonesToLevelGenerationArea()
{
	// For each Zone to use in initialisation:
	static AActor* ZoneTile;
	FVector DEFAULT_ZONE_SCALE = FVector(1.0f, 1.0f, 1.0f);

	// To offset the placement of zones (to fall within the level generation area):
	const FVector2D ZONE_POSITION_OFFSET = FVector2D(50.0f, 50.0f);
	
	// For the Zone Blueprints, as Actors:
	TArray<AActor*> ActorZones;
	UGameplayStatics::GetAllActorsOfClass(GEditor->GetEditorWorldContext()
		.World()->GetCurrentLevel(), AZone::StaticClass(), ActorZones);
	
	// To store the array of zones
	TArray<AZone*> LevelZones;
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
				
				ZoneTile = UGameplayStatics::BeginSpawningActorFromBlueprint(GEditor->GetEditorWorldContext().World()->GetCurrentLevel(),
					GetSuitableZoneTile(FVector2D(CurrentPosition), LevelZones), LevelZoneTransform, false);

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
}

UBlueprint* UBalancedFPSLevelGeneratorTool::GetSuitableZoneTile(FVector2D CurrentPlacementPosition, TArray<AZone*> LevelZoneTiles)
{
	/**
	* The Edge colours to match a Zone against, to find a suitable Zone for this CurrentPlacementPosition.
	* Zone Edge order:
	* First element: North Edge.
	* Second element: East Edge.
	* Third element: South Edge.
	* Fourth element: West Edge.
	*/
	std::vector<FPSLevelGeneratorEdge::EdgeColour> TargetEdgeColours = 
		std::vector<FPSLevelGeneratorEdge::EdgeColour>(size_t(AZone::DEFAULT_ZONE_EDGE_COUNT));

	// The Zone sub-set to choose a Zone-tile from:
	TArray<AZone*> ZoneSubSet;
	// Make sure the sub-set is clear (so as to not chooose an invalid zone):
	ZoneSubSet.Empty();

	// Then fill it with all of the zones (these will be narrowed down to the
	// final choice for this space, later):
	ZoneSubSet = LevelZoneTiles;

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
		for (int TargetEdgeColoursIterator = 0; TargetEdgeColoursIterator < TargetEdgeColours.size();
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

	int ZoneChoice = GetZoneChoiceIndex(ZoneSubSet, TargetEdgeColours);
	
	// Return the Blueprint that represents this Zone:
	for (int ZoneBlueprintIterator = 0; ZoneBlueprintIterator < LevelZoneTiles.Num(); ZoneBlueprintIterator++)
	{
		if (ZoneSubSet[ZoneChoice]->GetName() == LevelZoneTiles[ZoneBlueprintIterator]->GetName())
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
int UBalancedFPSLevelGeneratorTool::GetZoneChoiceIndex(TArray<AZone*>& ZoneSubsetReference,
	std::vector<FPSLevelGeneratorEdge::EdgeColour>& TargetEdgeColoursReference)
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
	for (int CurrentEdgePosition = TargetEdgeColoursReference.size(); CurrentEdgePosition > 0;
		CurrentEdgePosition--)
	{
		RNGResult = RandomDistribution(RNG);
		// Match against colourless:
		if (TargetEdgeColoursReference[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Colourless)
		{
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstColourless(ZoneSubsetReference, CurrentEdgePosition);
		}
		// Match against blue:
		else if (TargetEdgeColoursReference[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Blue)
		{
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstBlue(ZoneSubsetReference, CurrentEdgePosition);
		}
		// Match against green:
		else if (TargetEdgeColoursReference[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Green)
		{
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstGreen(ZoneSubsetReference, CurrentEdgePosition);
		}
		// Match against red:
		else if (TargetEdgeColoursReference[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Red)
		{
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstRed(ZoneSubsetReference, CurrentEdgePosition);
		}
		// Match against grey:
		else if (TargetEdgeColoursReference[CurrentEdgePosition] == FPSLevelGeneratorEdge::EdgeColour::Grey)
		{
			MatchingEdgeColours[CurrentEdgePosition] = MatchAgainstGrey(ZoneSubsetReference, CurrentEdgePosition);
		}
	}

	// Make sure a colourless edge has not been chosen (as there are no tiles with a colourless edge):
	for (int MatchingEdgeColoursIterator = 0; MatchingEdgeColoursIterator < MatchingEdgeColours.size();
		MatchingEdgeColoursIterator++)
	{
		if (MatchingEdgeColours[MatchingEdgeColoursIterator] == FPSLevelGeneratorEdge::EdgeColour::Colourless)
		{
			MatchingEdgeColours[MatchingEdgeColoursIterator] = MatchAgainstColourless(ZoneSubsetReference, RNGResult);
		}
	}

	// Check for a zone that matches these colours,
	// or if there is no absolute match, Find the closest match:
	bool AllEdgesMatch = true;
	int MatchingEdgeCount = 0;
	
	// For the indices of the edges to be considered,
	// if no absolute match has been found (with 
	// the key in this map indicating the tile index
	// and the value indicating the number of matching
	// edges):
	std::map<int, int> ConsideredEdges;

	for (int ZoneTileEdgeColoursIterator = 0; ZoneTileEdgeColoursIterator < ZonesEdgeColours.AllEdgeColours.size();
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
			ConsideredEdges.insert(std::pair<int, int>(ZoneTileEdgeColoursIterator, MatchingEdgeCount));
		}
	}

	// No absolute match has been found,
	// so find the next best value from
	// ConsideredEdges:
	
	if (NewZoneChoice == -1)
	{
		// Use RandomDistribution and RNG to choose from ClosestMatchingEdges:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ZonesEdgeColours.AllEdgeColours.size() - 1);
		RNG.seed(time(NULL));
		RandomDistribution(RNG);

		NewZoneChoice = RandomDistribution(RNG);
		/**
		ISSUE HERE SEEMS TO BE THE USE OF AUTO OR AN ITERATOR,
		RESOLVE THISG(£UAGUA"(G"UA(
		// Only for the indices:
		std::vector<int> ClosestMatchingEdges;
		int HighestMatchingEdgeCount = 0;

		for (auto ConsideredEdgesIterator = ConsideredEdges.begin(); 
			ConsideredEdgesIterator != ConsideredEdges.end();
			ConsideredEdgesIterator++)
		{
			if (HighestMatchingEdgeCount < ConsideredEdgesIterator->second)
			{
				HighestMatchingEdgeCount = ConsideredEdgesIterator->second;
			}			
		}

		for (auto ConsideredEdgesIterator = ConsideredEdges.begin();
			ConsideredEdgesIterator != ConsideredEdges.end();
			ConsideredEdgesIterator++)
		{
			if (ConsideredEdgesIterator->second == HighestMatchingEdgeCount)
			{
				ClosestMatchingEdges.push_back(ConsideredEdgesIterator->first);
			}			
		}

		// Use RandomDistribution and RNG to choose from ClosestMatchingEdges:
		RandomDistribution = std::uniform_int_distribution<int>(0,
			ClosestMatchingEdges.size() - 1);
		RNG.seed(time(NULL));
		RandomDistribution(RNG);

		NewZoneChoice = ClosestMatchingEdges[RandomDistribution(RNG)];
		*/
	}
	
	return NewZoneChoice;
}

void UBalancedFPSLevelGeneratorTool::UpdateValidZonesForPlacement(TArray<AZone*>& ZoneSubsetReference,
	FPSLevelGeneratorEdge::EdgeColour TargetEdgeColour, int CurrentEdgePosition)
{
	// Check if there is a match against the current edge-colour:
	for (int ZoneIterator = 0; ZoneIterator < ZoneSubsetReference.Num(); ZoneIterator++)
	{
		if (ZoneSubsetReference[ZoneIterator]->GetZoneEdgeColours()[CurrentEdgePosition]
			!= TargetEdgeColour)
		{
			ZoneSubsetReference[ZoneIterator]->SetZoneValidForPlacement(false);			
		}
		else if (ZoneSubsetReference[ZoneIterator]->GetZoneEdgeColours()[CurrentEdgePosition]
			== TargetEdgeColour)
		{
			ZoneSubsetReference[ZoneIterator]->SetZoneValidForPlacement(true);
		}
	}
}

// For matching against certain Edges:
FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstColourless(TArray<AZone*>& ZoneSubsetReference, int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult;

	// Remove all Zones without a blue Edge at CurrentEdgePosition:
	if (RNGResult < COLOURLESS_TO_BLUE)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}
	// Remove all Zones without a green Edge at CurrentEdgePosition:
	else if (RNGResult >= COLOURLESS_TO_BLUE &&
		RNGResult < COLOURLESS_TO_BLUE + COLOURLESS_TO_GREEN)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Remove all Zones without a red Edge at CurrentEdgePosition:
	else if (RNGResult >= COLOURLESS_TO_BLUE + COLOURLESS_TO_GREEN &&
		RNGResult < COLOURLESS_TO_BLUE + COLOURLESS_TO_GREEN + COLOURLESS_TO_RED)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Remove all Zones without a grey Edge at CurrentEdgePosition:
	else if (RNGResult >= COLOURLESS_TO_BLUE + COLOURLESS_TO_GREEN + COLOURLESS_TO_RED &&
		RNGResult < COLOURLESS_TO_BLUE + COLOURLESS_TO_GREEN + COLOURLESS_TO_RED + COLOURLESS_TO_GREY)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstGrey(TArray<AZone*>& ZoneSubsetReference, int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult;

	// Remove all Zones without a blue Edge at CurrentEdgePosition:
	if (RNGResult < GREY_TO_BLUE)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}
	// Remove all Zones without a green Edge at CurrentEdgePosition:
	else if (RNGResult >= GREY_TO_BLUE &&
		RNGResult < GREY_TO_BLUE + GREY_TO_GREEN)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Remove all Zones without a red Edge at CurrentEdgePosition:
	else if (RNGResult >= GREY_TO_BLUE + GREY_TO_GREEN &&
		RNGResult < GREY_TO_BLUE + GREY_TO_GREEN + GREY_TO_RED)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Remove all Zones without a grey Edge at CurrentEdgePosition:
	else if (RNGResult >= GREY_TO_BLUE + GREY_TO_GREEN + GREY_TO_RED &&
		RNGResult < GREY_TO_BLUE + GREY_TO_GREEN + GREY_TO_RED + GREY_TO_GREY)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstRed(TArray<AZone*>& ZoneSubsetReference, int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult;

	// Remove all Zones without a blue Edge at CurrentEdgePosition:
	if (RNGResult < RED_TO_BLUE)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}
	// Remove all Zones without a green Edge at CurrentEdgePosition:
	else if (RNGResult >= RED_TO_BLUE &&
		RNGResult < RED_TO_BLUE + RED_TO_GREEN)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Remove all Zones without a red Edge at CurrentEdgePosition:
	else if (RNGResult >= RED_TO_BLUE + RED_TO_GREEN &&
		RNGResult < RED_TO_BLUE + RED_TO_GREEN + RED_TO_RED)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Remove all Zones without a grey Edge at CurrentEdgePosition:
	else if (RNGResult >= RED_TO_BLUE + RED_TO_GREEN + RED_TO_RED &&
		RNGResult < RED_TO_BLUE + RED_TO_GREEN + RED_TO_RED + 
		RED_TO_GREY + RED_TO_COLOURLESS)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstGreen(TArray<AZone*>& ZoneSubsetReference, int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult;

	// Remove all Zones without a blue Edge at CurrentEdgePosition:
	if (RNGResult < GREEN_TO_BLUE)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}
	// Remove all Zones without a green Edge at CurrentEdgePosition:
	else if (RNGResult >= GREEN_TO_BLUE &&
		RNGResult < GREEN_TO_BLUE + GREEN_TO_GREEN)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Remove all Zones without a red Edge at CurrentEdgePosition:
	else if (RNGResult >= GREEN_TO_BLUE + GREEN_TO_GREEN &&
		RNGResult < GREEN_TO_BLUE + GREEN_TO_GREEN + GREEN_TO_RED)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Remove all Zones without a grey Edge at CurrentEdgePosition:
	else if (RNGResult >= GREEN_TO_BLUE + GREEN_TO_GREEN + GREEN_TO_RED &&
		RNGResult < GREEN_TO_BLUE + GREEN_TO_GREEN + GREEN_TO_RED +
		GREEN_TO_GREY + GREEN_TO_COLOURLESS)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}

	return ColourResult;
}

FPSLevelGeneratorEdge::EdgeColour UBalancedFPSLevelGeneratorTool::MatchAgainstBlue(TArray<AZone*>& ZoneSubsetReference, int RNGResult)
{
	// The colour to match against this colour, as per what the RNGResult is:
	FPSLevelGeneratorEdge::EdgeColour ColourResult;

	// Remove all Zones without a green Edge at CurrentEdgePosition:
	if (RNGResult < BLUE_TO_GREEN)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Green;
	}
	// Remove all Zones without a red Edge at CurrentEdgePosition:
	else if (RNGResult >= BLUE_TO_GREEN &&
		RNGResult < BLUE_TO_GREEN + BLUE_TO_RED)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Red;
	}
	// Remove all Zones without a grey Edge at CurrentEdgePosition:
	else if (RNGResult >= BLUE_TO_GREEN + BLUE_TO_RED &&
		RNGResult < BLUE_TO_GREEN + BLUE_TO_RED + BLUE_TO_GREY + BLUE_TO_COLOURLESS)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Grey;
	}
	// Remove all Zones without a blue Edge at CurrentEdgePosition:
	else if (RNGResult >= 100 - BLUE_TO_BLUE)
	{
		ColourResult = FPSLevelGeneratorEdge::EdgeColour::Blue;
	}

	return ColourResult;
}
