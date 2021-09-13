// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaySlate.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_ActActionBlueprint.h"
#include "Modules/ModuleManager.h"

class IAssetTools;
class FAssetToolsModule;
IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);


void FPlaySlateModule::StartupModule()
{
	// ** 用AssetTools注册AssetTypeActions_ActActionBlueprint工厂类型
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	TSharedRef<IAssetTypeActions> Action = MakeShareable(new FAssetTypeActions_ActActionBlueprint());
	AssetTools.RegisterAssetTypeActions(Action);
	CreatedAssetTypeActions.Add(Action);
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