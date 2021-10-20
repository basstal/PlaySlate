#include "AssetTypeActions_ActActionSequence.h"

#include "PlaySlate.h"
#include "NovaAct/ActActionSequenceEditor.h"
#include "NovaAct/Assets/ActActionSequence.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FText FAssetTypeActions_ActActionSequence::GetName() const
{
	return NSLOCTEXT("AssetTypeActions", "AssetTypeActions_ActActionSequence", "Act Action Sequence");
}

FColor FAssetTypeActions_ActActionSequence::GetTypeColor() const
{
	return FColor(100, 123, 72);
}

UClass* FAssetTypeActions_ActActionSequence::GetSupportedClass() const
{
	return UActActionSequence::StaticClass();
}

void FAssetTypeActions_ActActionSequence::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UActActionSequence* ActActionSequence = Cast<UActActionSequence>(*ObjIt);

		if (ActActionSequence)
		{
			/**
			 * TODO:这里是否允许多开？目前不允许
			 */
			TSharedRef<FActActionSequenceEditor> SequenceEditor(new FActActionSequenceEditor(ActActionSequence));
			SequenceEditor->InitActActionSequenceEditor(EditWithinLevelEditor);
			FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
			PlaySlateModule.ActActionSequenceEditor = SequenceEditor;
			return;
		}
	}
}

uint32 FAssetTypeActions_ActActionSequence::GetCategories()
{
	/**
	 * 资源分类，隶属于哪个资源类型
	 */
	return EAssetTypeCategories::Gameplay;
}

bool FAssetTypeActions_ActActionSequence::HasActions(const TArray<UObject*>& InObjects) const
{
	/**
	 * 是否在AssetActions中有自定义的Action
	 */
	return false;
}


#undef LOCTEXT_NAMESPACE
