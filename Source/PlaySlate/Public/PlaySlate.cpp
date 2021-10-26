// Copyright Epic Games, Inc. All Rights Reserved.
#include "PlaySlate.h"

#include "NovaAct/Assets/AssetTypeActions/AssetTypeActions_ActActionSequence.h"
#include "NovaBlueprint/Assets/AssetTypeActions/AssetTypeActions_ActActionBlueprint.h"

#include "AssetToolsModule.h"

DEFINE_LOG_CATEGORY(LogNovaAct)

IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);

#define LOCTEXT_NAMESPACE "NovaAct"


void FPlaySlateModule::StartupModule()
{
	// ** 用AssetTools注册FAssetTypeActions_ActActionBlueprint工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionBlueprint));
	// ** 用AssetTools注册FAssetTypeActions_ActActionSequence工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionSequence));
}

void FPlaySlateModule::ShutdownModule()
{
	if (const FAssetToolsModule* AssetToolsModule = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		IAssetTools& AssetTools = AssetToolsModule->Get();
		for (const TSharedRef<IAssetTypeActions>& AssetTypeAction : RegisteredAssetTypeActions)
		{
			AssetTools.UnregisterAssetTypeActions(AssetTypeAction);
		}
	}
	RegisteredAssetTypeActions.Empty();
}

void FPlaySlateModule::RegisterAssetTypeActions(const TSharedRef<IAssetTypeActions>& InAssetTypeActions)
{
	// ** NOTE:该模块必须在AssetTools模块之后初始化
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(InAssetTypeActions);
	RegisteredAssetTypeActions.Add(InAssetTypeActions);
}

#undef LOCTEXT_NAMESPACE
