// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaySlate.h"

#include "AssetToolsModule.h"
#include "Assets/AssetTypeActions_ActActionBlueprint.h"
#include "Assets/AssetTypeActions_ActActionSequence.h"
#include "Modules/ModuleManager.h"

class IAssetTools;
class FAssetToolsModule;
IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);


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