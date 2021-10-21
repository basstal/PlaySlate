// Copyright Epic Games, Inc. All Rights Reserved.
#include "PlaySlate.h"

#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"
#include "NovaAct/Assets/AssetTypeActions/AssetTypeActions_ActActionSequence.h"
#include "NovaBlueprint/Assets/AssetTypeActions/AssetTypeActions_ActActionBlueprint.h"

#include "AssetToolsModule.h"

DEFINE_LOG_CATEGORY(LogActAction)

IMPLEMENT_MODULE(FPlaySlateModule, PlaySlate);

#define LOCTEXT_NAMESPACE "NovaAct"

void FPlaySlateModule::StartupModule()
{
	// ** 用AssetTools注册FAssetTypeActions_ActActionBlueprint工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionBlueprint));
	// ** 用AssetTools注册FAssetTypeActions_ActActionSequence工厂类型
	RegisterAssetTypeActions(MakeShareable(new FAssetTypeActions_ActActionSequence));

	TSharedRef<TDataBinding<ActActionSequence::FActActionTimeSliderArgs>> TimeSliderArgsModel = NovaDataBinding::Create<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs");
	TSharedPtr<ActActionSequence::FActActionTimeSliderArgs> TimeSliderArgs = MakeShareable(new ActActionSequence::FActActionTimeSliderArgs());
	TimeSliderArgsModel->SetData(TimeSliderArgs);
	TimeSliderArgsModel->Bind(FName("Test"),
	                          [](TSharedPtr<ActActionSequence::FActActionTimeSliderArgs> Data)
	                          {
		                          UE_LOG(LogActAction, Log, TEXT("DataChanged Data->PlaybackRange : %d"), Data->DBTestValue)
	                          });
	TimeSliderArgsModel->Trigger();
	TimeSliderArgs->DBTestValue = 999;
	TimeSliderArgsModel->Trigger();
	TSharedRef<TDataBinding<ActActionSequence::FActActionTimeSliderArgs>> ModelAgain = NovaDataBinding::GetOrCreate<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs");
	UE_LOG(LogActAction, Log, TEXT("TimeSliderArgsModel equal ModelAgain : %d"), ModelAgain == TimeSliderArgsModel);
	ModelAgain = NovaDataBinding::GetOrCreate<ActActionSequence::FActActionTimeSliderArgs>("TimeSliderArgs1");
	UE_LOG(LogActAction, Log, TEXT("TimeSliderArgsModel equal ModelAgain next : %d"), ModelAgain == TimeSliderArgsModel);
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
