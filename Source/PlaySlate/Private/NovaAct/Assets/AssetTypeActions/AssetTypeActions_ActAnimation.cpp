#include "AssetTypeActions_ActAnimation.h"

// #include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FText FAssetTypeActions_ActAnimation::GetName() const
{
	return LOCTEXT("AssetTypeActions_ActActionSequence", "Act Action Sequence");
}

FColor FAssetTypeActions_ActAnimation::GetTypeColor() const
{
	return FColor(100, 123, 72);
}

UClass* FAssetTypeActions_ActAnimation::GetSupportedClass() const
{
	return UActAnimation::StaticClass();
}

void FAssetTypeActions_ActAnimation::OpenAssetEditor(const TArray<UObject*>& InObjects, TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		UActAnimation* ActAnimation = Cast<UActAnimation>(*ObjIt);

		if (ActAnimation)
		{
			/**
			 * TODO:这里是否允许多开？目前不允许
			 */
			TSharedPtr<FNovaActEditor> NovaActEditor(new FNovaActEditor(ActAnimation));
			NovaDB::CreateSP("NovaActEditor", NovaActEditor);
			NovaActEditor->CreateEditorWindow(EditWithinLevelEditor);
			return;
		}
	}
}

uint32 FAssetTypeActions_ActAnimation::GetCategories()
{
	/**
	 * 资源分类，隶属于哪个资源类型
	 */
	return EAssetTypeCategories::Gameplay;
}

bool FAssetTypeActions_ActAnimation::HasActions(const TArray<UObject*>& InObjects) const
{
	/**
	 * 是否在AssetActions中有自定义的Action
	 */
	return false;
}


#undef LOCTEXT_NAMESPACE
