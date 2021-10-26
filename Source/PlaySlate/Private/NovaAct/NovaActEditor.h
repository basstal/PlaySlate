#pragma once

#include "IAnimationEditor.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"

class FActAssetDetailsNotifyHook;
class FActViewport;
class UActAnimation;
class FActActionViewportClient;

class FActAssetDetailsBrain;

using namespace NovaDelegate;
using namespace NovaStruct;

/**
 * 资源编辑器的入口和管理者，提供一些工具方法和资源对象指针
 * 该对象会与编辑器的主页签一同释放
 */
class FNovaActEditor : public FWorkflowCentricApplication, public FGCObject, public FEditorUndoClient
{
public:
	FNovaActEditor(UActAnimation* InActAnimation);
	virtual ~FNovaActEditor() override;

	/**
	* 构造编辑器窗口，设置当前编辑的资源实例及相关参数
	*
	* @param InIToolkitHost 仅透传到对应接口，无实际作用
	*/
	void CreateEditorWindow(const TSharedPtr<IToolkitHost>& InIToolkitHost);

	//~Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	//~End FGCObject interface

	//~Begin FAssetEditorToolkit interface
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	//~End FAssetEditorToolkit interfaced

	/**
	 * 生成 Viewport Tab
	 *
	 * @param SpawnTabArgs
	 * @return 生成的 Widget
	 */
	TSharedRef<SDockTab> OnActViewportTabSpawn(const FSpawnTabArgs& SpawnTabArgs);
	/**
	 * 生成 EventTimeline Tab
	 *
	 * @param SpawnTabArgs
	 * @return 生成的 Widget
	 */
	TSharedRef<SDockTab> OnActEventTimelineTabSpawn(const FSpawnTabArgs& SpawnTabArgs);
	/**
	 * 生成 AssetDetails Tab
	 *
	 * @param SpawnTabArgs
	 * @return 生成的 Widget
	 */
	TSharedRef<SDockTab> OnActAssetDetailsTabSpawn(const FSpawnTabArgs& SpawnTabArgs);
	/**
	 * 设置AnimSequence的相关数据
	 *
	 * @param InActAnimation
	 */
	void OnAnimSequenceChanged(UActAnimation* InActAnimation);
protected:
	/** DataBinding */
	TSharedPtr<TDataBindingUObject<UActAnimation>> ActAnimationDB;           // 当前资源实例的数据绑定
	TSharedPtr<TDataBindingSP<FActEventTimelineArgs>> ActEventTimelineArgsDB;// EventTimeline 参数的数据绑定

	TSharedPtr<FActViewport> ActViewport;/** Viewport Controller，Editor没有销毁的情况下不会为空 */

	TSharedPtr<SDockTab> ActEventTimelineParentDockTab;/** EventTimeline Widget Container */
	TSharedPtr<SDockTab> ActViewportParentDockTab;     /** Viewport Widget Container */
	TSharedPtr<SDockTab> ActAssetDetailsParentDockTab; /** AssetDetails Widget Container */
};
