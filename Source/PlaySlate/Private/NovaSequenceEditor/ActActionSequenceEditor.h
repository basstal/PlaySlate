#pragma once

#include "IAnimationEditor.h"

class UActActionSequence;
class SActActionSequenceWidget;
class SActActionViewportWidget;
class FActActionSequenceController;

class FActActionSequenceEditor : public FWorkflowCentricApplication, public FGCObject, public FEditorUndoClient
{
public:
	virtual ~FActActionSequenceEditor() override;
	/**
	* 初始化ActActionSequence编辑器
	*
	* @param Mode							资源编辑模式
	* @param InitToolkitHost				当模式为WorldCentric时，编辑器实例由这个Host初始化
	* @param InActActionSequence			当前编辑的Sequence资源
	*/
	void InitActActionSequenceEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UActActionSequence* InActActionSequence);

	//~Begin FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override;
	//~End FGCObject interface


	//~Begin FAssetEditorToolkit interface
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual void RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager) override;
	//~End FAssetEditorToolkit interfaced

	// //~Begin FBaseToolkit interface
	// virtual void CreateEditorModeManager() override;
	// //~End FBaseToolkit interface

protected:
	/** Callback for spawning tabs. */
	TSharedRef<SDockTab> HandleTabManagerSpawnSequence(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> HandleTabManagerSpawnViewport(const FSpawnTabArgs& Args);

	/**
	 * TRANS_EN:Level sequence for our edit operation.
	 * Model模块
	 */
	UActActionSequence* ActActionSequencePtr;
	/**
	 * Sequence View模块
	 */
	TSharedPtr<SActActionSequenceWidget> SequenceMain;
	/**
	 * Viewport View模块
	 */
	TSharedPtr<SActActionViewportWidget> Viewport;
	/**
	 * Controller模块
	 */
	TSharedPtr<FActActionSequenceController> ActActionSequenceController;

	TSharedPtr<SDockTab> ViewportParentDockTab;
	// Tracking the active viewports in this editor.
	// TSharedPtr<FEditorViewportTabContent> ViewportTabContent;
};
