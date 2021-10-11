#pragma once

#include "IAnimationEditor.h"
#include "Controllers/Viewport/ActActionPreviewSceneController.h"

class UActActionSequence;
class FActActionViewportClient;
class FActActionSequenceController;

/**
 * ActActionSequence资源编辑器的入口和管理者，提供一些工具方法和资源对象指针
 * 该对象会与编辑器的主页签一同释放
 * 其他子Controller（例如ActActionSequenceController）保存的都是WeakPtr
 */
class FActActionSequenceEditor : public FWorkflowCentricApplication, public FGCObject, public FEditorUndoClient
{
public:
	FActActionSequenceEditor(UActActionSequence* InActActionSequence);
	virtual ~FActActionSequenceEditor() override;
	/**
	* 初始化编辑器，设置当前编辑的资源实例及相关参数
	*
	* @param InitToolkitHost				仅透传到对应接口，无实际作用
	*/
	void InitActActionSequenceEditor(const TSharedPtr<IToolkitHost>& InitToolkitHost);

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

protected:
	/**
	 * 当前编辑的资源实例
	 */
	UActActionSequence* ActActionSequence;
	/**
	 * Viewport Controller
	 */
	TSharedPtr<FActActionPreviewSceneController> ActActionPreviewSceneController;

	/**
	 * Sequence Controller
	 */
	TSharedPtr<FActActionSequenceController> ActActionSequenceController;

	TSharedPtr<SDockTab> ActActionSequenceWidgetParent;
	TSharedPtr<SDockTab> ActActionViewportWidgetParent;
public:
	TSharedPtr<FActActionPreviewSceneController> GetActActionPreviewSceneController() const
	{
		return ActActionPreviewSceneController;
	}
	UActActionSequence* GetActActionSequence() const
	{
		return ActActionSequence;
	}
};
