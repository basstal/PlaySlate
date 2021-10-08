#include "AssetTypeActions_ActActionSequence.h"

#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

FText FAssetTypeActions_ActActionSequence::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ActActionSequence", "Act Action Sequence");
}

FColor FAssetTypeActions_ActActionSequence::GetTypeColor() const
{
	return FColor(100,123,72);
}

UClass* FAssetTypeActions_ActActionSequence::GetSupportedClass() const
{
	return UActActionSequence::StaticClass();
}

void FAssetTypeActions_ActActionSequence::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UActActionSequence* ActActionSequence = Cast<UActActionSequence>(*ObjIt);

		if (ActActionSequence)
		{
			TSharedRef<FActActionSequenceEditor> SequenceEditor(new FActActionSequenceEditor());
			SequenceEditor->InitActActionSequenceEditor(Mode, EditWithinLevelEditor, ActActionSequence);
		}
	}
}

uint32 FAssetTypeActions_ActActionSequence::GetCategories()
{
	return EAssetTypeCategories::Gameplay;
}

bool FAssetTypeActions_ActActionSequence::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}


#undef LOCTEXT_NAMESPACE
