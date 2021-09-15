#pragma once

#include "IAnimationEditor.h"
#include "Assets/ActActionSequence.h"
#include "SWidget/ActActionSequenceMain.h"

class FActActionSequenceEditor : public FAssetEditorToolkit, public FGCObject, public FEditorUndoClient, public FTickableEditorObject
{
public:
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
	//~End FGCObject interface

	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~End FTickableEditorObject interface

	//~Begin FAssetEditorToolkit interface
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	//~End FAssetEditorToolkit interfaced

protected:
	/**
	 * TRANS_EN:Level sequence for our edit operation.
	 * Model模块
	 */
	UActActionSequence* ActActionSequence;
	/**
	 * View模块
	 */
	TSharedPtr<SActActionSequenceMain> SequenceMain;
	/**
	 * Controller模块
	 */
	TSharedPtr<FActActionSequenceController> ActActionSequenceController;
};
