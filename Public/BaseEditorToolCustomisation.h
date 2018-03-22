// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "Reply.h"

/**
 * 
 */
class BALANCEDFPSLEVELGENERATOR_API FBaseEditorToolCustomisation : public IDetailCustomization
{
	
public:
	
	// Functions/Methods:

	// 'IDetailCustomization interface':
	virtual void CustomizeDetails(IDetailLayoutBuilder&
		DetailBuilderReference) override;
	// 'End of IDetailCustomization interface'
	
	static TSharedRef<IDetailCustomization> MakeInstance();

	static FReply ExecuteToolCommand(IDetailLayoutBuilder* DetailBuilder,
		UFunction* MethodToExecute);
};
