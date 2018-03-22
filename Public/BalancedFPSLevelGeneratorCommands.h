// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "BalancedFPSLevelGeneratorStyle.h"

class FBalancedFPSLevelGeneratorCommands : public TCommands<FBalancedFPSLevelGeneratorCommands>
{
public:

	FBalancedFPSLevelGeneratorCommands()
		: TCommands<FBalancedFPSLevelGeneratorCommands>(TEXT("BalancedFPSLevelGenerator"), NSLOCTEXT("Contexts", "BalancedFPSLevelGenerator", "BalancedFPSLevelGenerator Plugin"), NAME_None, FBalancedFPSLevelGeneratorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
