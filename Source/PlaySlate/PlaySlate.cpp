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
	TSharedRef<IAssetTypeActions> ActActionSequence = MakeShared<FAssetTypeActions_ActActionSequence>();
	AssetTools.RegisterAssetTypeActions(ActActionSequence);
	CreatedAssetTypeActions.Add(ActActionSequence);

	// Register the editor modes
	FEditorModeRegistry::Get().RegisterMode<FActActionViewportEditMode>(ActActionSequence::ActActionViewportEditMode, LOCTEXT("ActActionEditMode", "ActAction Selection"), FSlateIcon(), false);

}

void FPlaySlateModule::ShutdownModule()
{
	if (FAssetToolsModule * AssetTools = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		for (auto AssetTypeAction : CreatedAssetTypeActions)
		{
			AssetTools->Get().UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

#undef LOCTEXT_NAMESPACE