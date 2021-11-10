#pragma once

#include "BlueprintEditor.h"

class FActActionBlueprintEditor : public FBlueprintEditor
{
public:
	/**
	 * 初始化ActActionLogic编辑器
	 *
	 * @param Mode							资源编辑模式
	 * @param InitToolkitHost				当模式为WorldCentric时，编辑器实例由这个Host初始化
	 * @param InBlueprints					当前编辑的蓝图
	 * @param bShouldOpenInDefaultsMode		是否用默认编辑模式打开
	 */
	void InitActActionBlueprintEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, const TArray<UBlueprint*>& InBlueprints, bool bShouldOpenInDefaultsMode);

	/**
	* 更新已打开的ActActionLogic的蓝图，确保其是最新的
	* 
	* @param	Blueprint	待更新的蓝图
	*/
	void EnsureActActionBlueprintIsUpToDate(UBlueprint* Blueprint);
public:
	//~Begin FBlueprintEditor Interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FText GetToolkitToolTipText() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual UBlueprint* GetBlueprintObj() const override;
	//~End FBlueprintEditor Interface
};
