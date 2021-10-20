#include "AssetTypeActions_ActActionBlueprint.h"

#include "PlaySlate.h"
#include "NovaBlueprint/Assets/ActActionBlueprint.h"
#include "NovaBlueprint/ActActionBlueprintEditor.h"
#include "NovaBlueprint/Controllers/AssetTypeActions/ActActionBlueprintFactory.h"

#include "BlueprintEditorSettings.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/MessageDialog.h"

class UActActionLogic;

#define LOCTEXT_NAMESPACE "NovaAct"

FText FAssetTypeActions_ActActionBlueprint::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ActActionBlueprint", "Act Action Blueprint");
}

FColor FAssetTypeActions_ActActionBlueprint::GetTypeColor() const
{
	return FColor(200, 100, 0);
}

UClass* FAssetTypeActions_ActActionBlueprint::GetSupportedClass() const
{
	return UActActionBlueprint::StaticClass();
}

void FAssetTypeActions_ActActionBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	UE_LOG(LogActAction, Log, TEXT("Mode : %d"), Mode);
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UActActionBlueprint* ActActionBlueprint = Cast<UActActionBlueprint>(*ObjIt);
		if (ActActionBlueprint)
		{
			bool bLetOpen = true;
			if (!ActActionBlueprint->ParentClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("FailedToLoadActActionBlueprintWithContinue",
				                                                                                   "Act Action Blueprint could not be loaded because it derives from an invalid class. Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue(it can crash the editor)?"));
			}
			if (bLetOpen)
			{
				TSharedRef<FActActionBlueprintEditor> NewEditor(new FActActionBlueprintEditor());

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(ActActionBlueprint);

				NewEditor->InitActActionBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(ActActionBlueprint));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("FailedToLoadActActionBlueprint", "Act Action Blueprint could not be loaded because it derives from an invalid class. Check to make sure the parent class for this blueprint hasn't been removed!"));
		}
	}
}

uint32 FAssetTypeActions_ActActionBlueprint::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}

UFactory* FAssetTypeActions_ActActionBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UActActionBlueprintFactory* Factory = NewObject<UActActionBlueprintFactory>();
	Factory->ParentClass = TSubclassOf<UActActionLogic>(*InBlueprint->GeneratedClass);
	return Factory;
}

bool FAssetTypeActions_ActActionBlueprint::ShouldUseDataOnlyEditor(const UBlueprint* Blueprint) const
{
	return FBlueprintEditorUtils::IsDataOnlyBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsLevelScriptBlueprint(Blueprint)
		&& !FBlueprintEditorUtils::IsInterfaceBlueprint(Blueprint)
		&& !Blueprint->bForceFullEditor
		&& !Blueprint->bIsNewlyCreated;
}


#undef LOCTEXT_NAMESPACE
