#include "AssetTypeActions_ActActionBlueprint.h"

#include "ActActionBlueprint.h"
#include "ActActionBlueprintEditor.h"
#include "ActActionBlueprintFactory.h"
#include "BlueprintEditorSettings.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_ActActionBlueprint::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ActActionBlueprint", "Act Action Blueprint");
}

FColor FAssetTypeActions_ActActionBlueprint::GetTypeColor() const
{
	return FColor(0, 255, 0);
}

UClass* FAssetTypeActions_ActActionBlueprint::GetSupportedClass() const
{
	return UActActionBlueprint::StaticClass();
}

void FAssetTypeActions_ActActionBlueprint::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UBlueprint* Blueprint = Cast<UBlueprint>(*ObjIt);
		if (Blueprint)
		{
			bool bLetOpen = true;
			if (!Blueprint->ParentClass)
			{
				bLetOpen = EAppReturnType::Yes == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("FailedToLoadActActionBlueprintWithContinue", "Act Action Blueprint could not be loaded because it derives from an invalid class. Check to make sure the parent class for this blueprint hasn't been removed! Do you want to continue(it can crash the editor)?"));
			}
			if (bLetOpen)
			{
				TSharedRef<FActActionBlueprintEditor> NewEditor(new FActActionBlueprintEditor());

				TArray<UBlueprint*> Blueprints;
				Blueprints.Add(Blueprint);

				NewEditor->InitActActionBlueprintEditor(Mode, EditWithinLevelEditor, Blueprints, ShouldUseDataOnlyEditor(Blueprint));
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
	return EAssetTypeCategories::Blueprint | EAssetTypeCategories::Gameplay;
}

UFactory* FAssetTypeActions_ActActionBlueprint::GetFactoryForBlueprintType(UBlueprint* InBlueprint) const
{
	UActActionBlueprintFactory* Factory = NewObject<UActActionBlueprintFactory>();
	Factory->ParentClass = TSubclassOf<UActActionBlueprint>(*InBlueprint->GeneratedClass);
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
