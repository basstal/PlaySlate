#pragma once

#include "IAnimationEditor.h"
#include "Common/NovaDelegate.h"
#include "Common/NovaDataBinding.h"

class FActAssetDetails;
class FActViewport;
class UActAnimation;
class FActActionViewportClient;
class FActEventTimeline;
class FActAssetDetailsBrain;

using namespace NovaDelegate;

/**
 * ActActionSequence资源编辑器的入口和管理者，提供一些工具方法和资源对象指针
 * 该对象会与编辑器的主页签一同释放
 * 其他子Controller（例如ActActionSequenceController）保存的都是WeakPtr
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

protected:
	TSharedPtr<TDataBindingUObject<UActAnimation>> ActAnimationDB;// 当前资源实例的数据绑定

	TSharedPtr<FActViewport> ActViewport;          /** Viewport Controller，Editor没有销毁的情况下不会为空 */
	TSharedPtr<FActEventTimeline> ActEventTimeline;/** Sequence Controller，Editor没有销毁的情况下不会为空 */
	TSharedPtr<FActAssetDetails> ActAssetDetails;  /** Details View Controller */

	TSharedPtr<SDockTab> ActEventTimelineWidgetParent;/** Sequence Widget Container */
	TSharedPtr<SDockTab> ActViewportWidgetParent;     /** Viewport Widget Container */
	TSharedPtr<SDockTab> ActAssetDetailsWidgetParent; /** Details Widget Container */
};
