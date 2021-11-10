#include "ActActionBlueprintEditor.h"

#include "Assets/ActActionBlueprint.h"
#include "Controllers/BlueprintGraph/ActActionBlueprintGraphSchema.h"

#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "NovaAct"


void FActActionBlueprintEditor::InitActActionBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode)
{
	InitBlueprintEditor(Mode, InitToolkitHost, InBlueprints, bShouldOpenInDefaultsMode);
	for (UBlueprint* const Blueprint : InBlueprints)
	{
		EnsureActActionBlueprintIsUpToDate(Blueprint);
	}
}

void FActActionBlueprintEditor::EnsureActActionBlueprintIsUpToDate(UBlueprint* Blueprint)
{
#if WITH_EDITORONLY_DATA
	for (UEdGraph* Graph : Blueprint->UbergraphPages)
	{
		// remove the default event graph if it exists
		if (Graph->GetName() == "EventGraph" && Graph->Nodes.Num() == 0)
		{
			check(!Graph->Schema->GetClass()->IsChildOf(UActActionBlueprintGraphSchema::StaticClass()));
			FBlueprintEditorUtils::RemoveGraph(Blueprint, Graph);
			break;
		}
	}
#endif
}


FName FActActionBlueprintEditor::GetToolkitFName() const
{
	return FName("ActActionEditor");
}

FText FActActionBlueprintEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ActAction Blueprint Editor");
}

FText FActActionBlueprintEditor::GetToolkitName() const
{
	const TArray<UObject*>& EditingObjs = GetEditingObjects();
	check(EditingObjs.Num() > 0);

	FFormatNamedArguments Args;
	const UObject* EditingObject = EditingObjs[0];
	Args.Add(TEXT("ObjectName"), FText::FromString(EditingObject->GetName()));
	return FText::Format(LOCTEXT("ActionToolkitName", "{ObjectName}"), Args);
}

FText FActActionBlueprintEditor::GetToolkitToolTipText() const
{
	const UObject* EditingObject = GetEditingObject();
	check(EditingObject != nullptr);
	return FAssetEditorToolkit::GetToolTipTextForObject(EditingObject);
}

FString FActActionBlueprintEditor::GetWorldCentricTabPrefix() const
{
	return TEXT("ActActionEditor");
}

FLinearColor FActActionBlueprintEditor::GetWorldCentricTabColorScale() const
{
	return FColor::White;
}

UBlueprint* FActActionBlueprintEditor::GetBlueprintObj() const
{
	const TArray<UObject*>& Objects = GetEditingObjects();
	for (auto Obj : Objects)
	{
		if (const auto Blueprint = Cast<UActActionBlueprint>(Obj))
		{
			return Blueprint;
		}
	}
	return nullptr;
}


#undef LOCTEXT_NAMESPACE
