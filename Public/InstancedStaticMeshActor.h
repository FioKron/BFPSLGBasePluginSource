// Copyright 1998-2013 Epic Games, Inc. All Rights Reserved.
// Taken from:
// https://answers.unrealengine.com/questions/23673/how-do-i-use-instancedstaticmeshes.html
// Answer by Rama.
#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "InstancedStaticMeshActor.generated.h"

/**
* 'An instance of a StaticMesh in a level
* Note that PostInitializeComponents() is not called for StaticMeshActors'
*/
UCLASS(Blueprintable, Abstract)
class AInstancedStaticMeshActor : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	
};
