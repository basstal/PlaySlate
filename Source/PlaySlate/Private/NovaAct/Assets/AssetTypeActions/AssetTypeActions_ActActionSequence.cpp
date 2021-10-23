#include "AssetTypeActions_ActActionSequence.h"

#include "PlaySlate.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"

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
	return UActAnimation::StaticClass();
}

void FAssetTypeActions_ActActionSequence::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UActAnimation* ActAnimation = Cast<UActAnimation>(*ObjIt);

		if (ActAnimation)
		{
			/**
			 * TODO:这里是否允许多开？目前不允许
			 */
			TSharedRef<FNovaActEditor> NovaActEditor(new FNovaActEditor(ActAnimation));
			NovaActEditor->CreateEditorWindow(EditWithinLevelEditor);
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
