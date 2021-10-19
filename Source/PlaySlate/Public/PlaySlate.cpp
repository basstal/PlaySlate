// Copyright Epic Games, Inc. All Rights Reserved.

#include "PlaySlate.h"

#include "AssetToolsModule.h"
#include "NovaSequenceEditor/Assets/AssetTypeActions/AssetTypeActions_ActActionSequence.h"
#include "NovaBlueprintEditor/Assets/AssetTypeActions/AssetTypeActions_ActActionBlueprint.h"

DEFINE_LOG_CATEGORY(LogActAction)

IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);

#define LOCTEXT_NAMESPACE "PlaySlate"

void FPlaySlateModule::StartupModule()
{
	// ** 用AssetTools注册FAssetTypeActions_ActActionBlueprint工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionBlueprint));
	// ** 用AssetTools注册FAssetTypeActions_ActActionSequence工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionSequence));
}

void FPlaySlateModule::ShutdownModule()
{
	if (const FAssetToolsModule* AssetTools = FModuleManager::GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		for (auto AssetTypeAction : CreatedAssetTypeActions)
		{
			AssetTools->Get().UnregisterAssetTypeActions(AssetTypeAction.ToSharedRef());
		}
	}
	CreatedAssetTypeActions.Empty();
}

void FPlaySlateModule::RegisterAssetTypeActions(TSharedRef<IAssetTypeActions> InAssetTypeActions)
{
	// ** NOTE:该模块必须在AssetTools模块之后初始化
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(InAssetTypeActions);
	CreatedAssetTypeActions.Add(InAssetTypeActions);
}


#undef LOCTEXT_NAMESPACE
