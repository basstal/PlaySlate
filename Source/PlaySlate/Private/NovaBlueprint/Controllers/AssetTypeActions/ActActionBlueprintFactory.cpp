#include "ActActionBlueprintFactory.h"

#include "NovaBlueprint/Assets/ActActionBlueprint.h"
#include "NovaBlueprint/Assets/ActActionLogic.h"
#include "NovaBlueprint/ActActionBlueprintCreateDialog.h"
#include "NovaBlueprint/Controllers/BlueprintGraph/ActActionBlueprintGraph.h"
#include "NovaBlueprint/Controllers/BlueprintGraph/ActActionBlueprintGraphSchema.h"

#include "BlueprintEditorSettings.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Misc/MessageDialog.h"
#include "Widgets/Layout/SUniformGridPanel.h"


#define LOCTEXT_NAMESPACE "NovaAct"

UActActionBlueprintFactory::UActActionBlueprintFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UActActionBlueprint::StaticClass();
	ParentClass = UActActionLogic::StaticClass();
}

bool UActActionBlueprintFactory::ConfigureProperties()
{
	TSharedPtr<SActActionBlueprintCreateDialog> Dialog = SNew(SActActionBlueprintCreateDialog);
	return Dialog->ConfigureProperties(this);
}

UObject* UActActionBlueprintFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return FactoryCreateNew(InClass, InParent, InName, Flags, Context, Warn, NAME_None);
}

UObject* UActActionBlueprintFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	// ** 构造的是ActionBlueprint或其子类
	check(InClass->IsChildOf(UActActionBlueprint::StaticClass()));

	if (BlueprintType == BPTYPE_Interface)
	{
		ParentClass = UInterface::StaticClass();
	}

	if (ParentClass == nullptr || !FKismetEditorUtilities::CanCreateBlueprintOfClass(ParentClass) || !ParentClass->IsChildOf(UActActionLogic::StaticClass()))
	{
		FFormatNamedArguments Args;
		Args.Add(TEXT("ClassName"), ParentClass != nullptr ? FText::FromString(ParentClass->GetName()) : LOCTEXT("Null", "(null)"));
		FMessageDialog::Open(EAppMsgType::Ok, FText::Format(LOCTEXT("CannotCreateActionBlueprint", "Cannot create a action blueprint '{ClassName}'"), Args));
		return nullptr;
	}

	UActActionBlueprint* NewBP = CastChecked<UActActionBlueprint>(FKismetEditorUtilities::CreateBlueprint(ParentClass, InParent, InName, BlueprintType, UActActionBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), CallingContext));
	if (NewBP)
	{
		UActActionBlueprint* ActionBP = UActActionBlueprint::FindRootActionBlueprint(NewBP);
		if (ActionBP == nullptr)
		{
			// const UEdGraphSchema_K2 * K2Schema = GetDefault<UEdGraphSchema_K2>();
			UEdGraph* NewGraph = FBlueprintEditorUtils::CreateNewGraph(NewBP, TEXT("Action Graph"), UActActionBlueprintGraph::StaticClass(), UActActionBlueprintGraphSchema::StaticClass());
#if WITH_EDITORONLY_DATA
			if (NewBP->UbergraphPages.Num())
			{
				FBlueprintEditorUtils::RemoveGraphs(NewBP, NewBP->UbergraphPages);
			}
#endif
			FBlueprintEditorUtils::AddUbergraphPage(NewBP, NewGraph);
			NewBP->LastEditedDocuments.Add(NewGraph);
			NewGraph->bAllowDeletion = false;

			UBlueprintEditorSettings* Settings = GetMutableDefault<UBlueprintEditorSettings>();
			if (Settings && Settings->bSpawnDefaultBlueprintNodes)
			{
				// int32 NodePositionY = 0;
				// ** 默认事件结点
				// FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewGraph, FName(TEXT("K2_ActivateAbility")), UAction::StaticClass(), NodePositionY);
				// FKismetEditorUtilities::AddDefaultEventNode(NewBP, NewGraph, FName(TEXT("K2_OnEndAbility")), UAction::StaticClass(), NodePositionY);
			}
		}
	}
	return NewBP;
}

#undef LOCTEXT_NAMESPACE
