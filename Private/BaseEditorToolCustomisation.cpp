// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEditorToolCustomisation.h"
#include "Button.h"
#include "PropertyEditing.h"

#define LOCTEXT_NAMESPACE "EditorTools"

TSharedRef<IDetailCustomization> FBaseEditorToolCustomisation::MakeInstance()
{
	return MakeShareable(new FBaseEditorToolCustomisation());
}

void FBaseEditorToolCustomisation::CustomizeDetails(IDetailLayoutBuilder&
	DetailBuilderReference)
{
	TSet<UClass*> Classes;

	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomised;
	DetailBuilderReference.GetObjectsBeingCustomized(/*'out'*/
		ObjectsBeingCustomised);

	for (auto WeakObject : ObjectsBeingCustomised)
	{
		if (UObject* Instance = WeakObject.Get())
		{
			Classes.Add(Instance->GetClass());
		}
	}

	// 'Create a commands category':
	IDetailCategoryBuilder& Category = DetailBuilderReference.EditCategory(
		"Commands");

	// 'Create a button for each element':
	for (UClass* Class : Classes)
	{
		// Setting up a button for each function:
		for (TFieldIterator<UFunction> FunctionIterator(Class); FunctionIterator;
			++FunctionIterator)
		{
			UFunction* Function = *FunctionIterator;

			const FString FunctionName = Function->GetName();
			const FText ButtonCaption = FText::FromString(FunctionName);
			const FText FilterString = FText::FromString(FunctionName);

			Category.AddCustomRow(FilterString)
			.ValueContent()
			[
				SNew(SButton)
				.Text(ButtonCaption)
				.OnClicked(FOnClicked::CreateStatic(&FBaseEditorToolCustomisation::ExecuteToolCommand, &DetailBuilderReference, Function))
			];
			
		}
	}

}

FReply FBaseEditorToolCustomisation::ExecuteToolCommand(IDetailLayoutBuilder* DetailBuilder, UFunction* MethodToExecute)
{
	TArray<TWeakObjectPtr<UObject>> ObjectsBeingCustomised;
	DetailBuilder->GetObjectsBeingCustomized(
		/* 'out' */ObjectsBeingCustomised);

	for (auto WeakObject : ObjectsBeingCustomised)
	{
		if (UObject* Instance = WeakObject.Get())
		{
			Instance->CallFunctionByNameWithArguments(
				*MethodToExecute->GetName(), *GLog,
				nullptr, true);
		}
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE