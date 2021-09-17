// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogActAction, Log, All)

class IAssetTypeActions;

class FPlaySlateModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	/**
	* 所有已注册的资源类型，登记在这里以便在模块关闭时反注册
	*/
	TArray<TSharedPtr<IAssetTypeActions>> CreatedAssetTypeActions;
};
