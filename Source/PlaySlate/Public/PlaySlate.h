// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once


class IAssetTypeActions;

DECLARE_LOG_CATEGORY_EXTERN(LogActAction, Log, All)

class FPlaySlateModule : public IModuleInterface
{
public:
	//~Begin IModuleInterface interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End IModuleInterface interface

	/**
	 * @param InAssetTypeActions 待注册的IAssetTypeActions资源类型
	 */
	void RegisterAssetTypeActions(TSharedRef<IAssetTypeActions> InAssetTypeActions);

protected:
	/**
	* 所有已注册的资源类型，登记在这里以便在模块关闭时反注册
	*/
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
};
