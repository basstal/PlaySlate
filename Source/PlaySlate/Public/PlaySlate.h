// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

class IAssetTypeActions;
class FActActionSequenceEditor;

DECLARE_LOG_CATEGORY_EXTERN(LogActAction, Log, All)

class FPlaySlateModule : public IModuleInterface
{
public:
	//~Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End IModuleInterface interface

	/**
	 * 注册AssetTypeActions以便能够检索到对应的资源类型
	 * 
	 * @param InAssetTypeActions 待注册的IAssetTypeActions资源类型
	 */
	void RegisterAssetTypeActions(const TSharedRef<IAssetTypeActions>& InAssetTypeActions);

protected:
	/** 所有已注册的资源类型，登记在这里以便在模块关闭时反注册 */
	TArray<TSharedRef<IAssetTypeActions>> RegisteredAssetTypeActions;
public:
	/** 对Editor的弱引用 */
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
};
