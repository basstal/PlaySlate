#pragma once

#include "IAnimationEditor.h"
#include "Utils/ActActionDelegates.h"

class FActActionPreviewSceneController;

class UActActionSequence;

class FActActionViewportClient;

class FActActionSequenceController;

class FActActionDetailsViewController;

/**
 * ActActionSequence资源编辑器的入口和管理者，提供一些工具方法和资源对象指针
 * 该对象会与编辑器的主页签一同释放
 * 其他子Controller（例如ActActionSequenceController）保存的都是WeakPtr
 */
class FActActionSequenceEditor : public FWorkflowCentricApplication, public FGCObject, public FEditorUndoClient, public FNotifyHook
{
public:
	FActActionSequenceEditor(UActActionSequence* InActActionSequence);

	virtual ~FActActionSequenceEditor() override;

	/**
	* 初始化编辑器，设置当前编辑的资源实例及相关参数
	*
	* @param InitToolkitHost 仅透传到对应接口，无实际作用
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

	//~Begin FNotifyHook interface
	virtual void NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) override;

	//~End FNotifyHook interface

	/**
	 * 根据AnimBlueprint初始化Viewport显示内容
	 *
	 * @param AnimBlueprint 被初始化的AnimBlueprint资源
	 */
	void InitAnimBlueprint(UAnimBlueprint* AnimBlueprint);

	/**
	 * @return 获得当前资源使用的Tick帧率
	 */
	FFrameRate GetTickResolution() const;

	/** @param InAnimSequence 设置当前资源的AnimSequence实例 */
	void SetAnimSequence(UAnimSequence* InAnimSequence) const;

	/**
	 * 添加一个攻击盒到数据中
	 */
	void AddHitBox() const;

protected:
	/** 当前编辑的资源实例 */
	UActActionSequence* ActActionSequence;

	/** Viewport Controller，Editor没有销毁的情况下不会为空 */
	TSharedPtr<FActActionPreviewSceneController> ActActionPreviewSceneController;

	/** Sequence Controller，Editor没有销毁的情况下不会为空 */
	TSharedPtr<FActActionSequenceController> ActActionSequenceController;

	/** Details View Controller */
	TSharedPtr<FActActionDetailsViewController> ActActionDetailsViewController;

	/** Sequence Widget Container */
	TSharedPtr<SDockTab> ActActionSequenceWidgetParent;

	/** Viewport Widget Container */
	TSharedPtr<SDockTab> ActActionViewportWidgetParent;

	/** Details Widget Container */
	TSharedPtr<SDockTab> ActActionDetailsViewWidgetParent;

	/** 动画播放的帧区间 */
	TRange<FFrameNumber> PlaybackRange;

	/** User-defined selection range. */
	TRange<FFrameNumber> SelectionRange;

public:
	TSharedRef<FActActionSequenceController> GetActActionSequenceController() const
	{
		return ActActionSequenceController.ToSharedRef();
	}

	TSharedRef<FActActionPreviewSceneController> GetActActionPreviewSceneController() const
	{
		return ActActionPreviewSceneController.ToSharedRef();
	}

	UActActionSequence* GetActActionSequence() const
	{
		return ActActionSequence;
	}

	TRange<FFrameNumber> GetSelectionRange() const
	{
		return SelectionRange;
	}

	TRange<FFrameNumber> GetPlaybackRange() const
	{
		return PlaybackRange;
	}

	void SetPlaybackRange(TRange<FFrameNumber> InRange)
	{
		if (ensure(InRange.HasLowerBound() && InRange.HasUpperBound()))
		{
			PlaybackRange = InRange;
		}
	}

	/** 资源属性修改的多播事件 */
	ActActionSequence::OnAssetPropertiesModifiedMulticastDelegate OnAssetPropertiesModified;
	/** 攻击盒修改的多播事件 */
	ActActionSequence::OnHitBoxesChangedMulticastDelegate OnHitBoxesChanged;
};
