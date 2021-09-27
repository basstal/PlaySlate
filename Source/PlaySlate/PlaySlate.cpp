// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaySlate.h"

#include "AssetToolsModule.h"
#include "EditorModeRegistry.h"
#include "Assets/AssetTypeActions_ActActionBlueprint.h"
#include "Assets/AssetTypeActions_ActActionSequence.h"
#include "Editor/ActActionViewportEditMode.h"
#include "Modules/ModuleManager.h"
#include "Utils/ActActionViewportUtil.h"

class IAssetTools;
class FAssetToolsModule;
class FActActionAnimInstanceTrack;
DEFINE_LOG_CATEGORY(LogActAction)

IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);

#define LOCTEXT_NAMESPACE "PlaySlate"

void FPlaySlateModule::StartupModule()
{
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	// ** 用AssetTools注册FAssetTypeActions_ActActionBlueprint工厂类型
	TSharedRef<IAssetTypeActions> ActActionBlueprint = MakeShareable(new FAssetTypeActions_ActActionBlueprint());
	AssetTools.RegisterAssetTypeActions(ActActionBlueprint);
	CreatedAssetTypeActions.Add(ActActionBlueprint);

	// ** 用AssetTools注册FAssetTypeActions_ActActionSequence工厂类型
	TSharedRef<IAssetTypeActions> ActActionSequence = MakeShareable(new FAssetTypeActions_ActActionSequence());
	AssetTools.RegisterAssetTypeActions(ActActionSequence);
	CreatedAssetTypeActions.Add(ActActionSequence);

	// Register the editor modes
	FEditorModeRegistry::Get().RegisterMode<FActActionViewportEditMode>(ActActionSequence::ActActionViewportEditMode, LOCTEXT("ActActionEditMode", "ActAction Selection"), FSlateIcon(), false);

}

// ** TODO:改为通过接口直接添加TrackEditor的引用
FDelegateHandle FPlaySlateModule::RegisterTrackEditor(ActActionSequence::OnCreateTrackEditorDelegate InOnCreateTrackEditor, TArrayView<ActActionSequence::FActActionAnimatedPropertyKey> AnimatedPropertyTypes)
{
	TrackEditorDelegates.Add(InOnCreateTrackEditor);
	for (const ActActionSequence::FActActionAnimatedPropertyKey& Key : AnimatedPropertyTypes)
	{
		PropertyAnimators.Add(Key);
	}
	FDelegateHandle Handle = InOnCreateTrackEditor.GetHandle();
	if (AnimatedPropertyTypes.Num() > 0)
	{
		ActActionSequence::FActActionAnimatedTypeCache CachedTypes;
		CachedTypes.FactoryHandle = Handle;
		for (const ActActionSequence::FActActionAnimatedPropertyKey& Key : AnimatedPropertyTypes)
		{
			CachedTypes.AnimatedTypes.Add(Key);
		}
		AnimatedTypeCache.Add(CachedTypes);
	}
	return Handle;
}

void FPlaySlateModule::UnRegisterTrackEditor(FDelegateHandle InHandle)
{
	TrackEditorDelegates.RemoveAll([=](const ActActionSequence::OnCreateTrackEditorDelegate& Delegate) { return Delegate.GetHandle() == InHandle; });
	int32 CacheIndex = AnimatedTypeCache.IndexOfByPredicate([=](const ActActionSequence::FActActionAnimatedTypeCache& In) { return In.FactoryHandle == InHandle; });
	if (CacheIndex != INDEX_NONE)
	{
		for (const ActActionSequence::FActActionAnimatedPropertyKey& Key : AnimatedTypeCache[CacheIndex].AnimatedTypes)
		{
			PropertyAnimators.Remove(Key);
		}
		AnimatedTypeCache.RemoveAtSwap(CacheIndex);
	}
}

void FPlaySlateModule::ShutdownModule()
{
	if (FAssetToolsModule* AssetTools = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		for (auto AssetTypeAction : CreatedAssetTypeActions)
		{
			AssetTools->Get().UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE
