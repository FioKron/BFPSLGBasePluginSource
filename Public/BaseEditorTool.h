// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseEditorTool.generated.h"

/**
 * This class can be used by all tools, shown in the editor menu
 */
UCLASS(Blueprintable, Abstract)
class BALANCEDFPSLEVELGENERATOR_API UBaseEditorTool : public UObject
{
	GENERATED_BODY()
	
public:

	// Properties:

	// 'This text will be displayed at the top of the tool dialog, when this 
	// tool is active':
	UPROPERTY(EditAnywhere, Category = Hidden)
	FText HelpText;
};
