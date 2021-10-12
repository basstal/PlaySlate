#pragma once

#include "Utils/ActActionPlaybackUtil.h"
#include "Utils/ActActionSequenceUtil.h"

#include "IContentBrowserSingleton.h"

class FActActionSequenceEditor;
class FActActionTimeSliderController;
class SActActionSequenceWidget;
class ASkeletalMeshActor;
class UActActionSequence;
class FActActionSequenceTreeViewNode;
class SActActionViewportWidget;
/**
 * 整个Sequence的主要控制器
 * 对应的View模块为SActActionSequenceMain
 */
class FActActionSequenceController : public TSharedFromThis<FActActionSequenceController>, FTickableEditorObject
{
public:
	FActActionSequenceController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionSequenceController() override;

	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~End FTickableEditorObject interface

	/**
	 * 调用已注册的TrackEditor的Create代理，并收集创建的TrackEditor实例
	 */
	void ExecuteTrackEditorCreateDelegate();
	/**
	 * TODO:
	 */
	void UpdateTimeBases();
	/**
	 * 构建AddTrack菜单的内容
	 *
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	void BuildAddTrackMenu(FMenuBuilder& MenuBuilder);
	/**
	 * @return 当前时间轴的显示范围
	 */
	ActActionSequence::FActActionAnimatedRange GetViewRange() const;
	
	
	void AddAnimMontageTrack(UAnimMontage* AnimMontage);
	void SetPlaybackStatus(ActActionSequence::EPlaybackType InPlaybackStatus);
	void EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange, bool bHasJumped = false);
	void Pause();
	/** Stop the sequencer from auto-scrolling */
	void StopAutoscroll();

	void MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams);
	TSet<FFrameNumber> GetVerticalFrames() const;
	void SetMarkedFrame(int32 InMarkIndex, FFrameNumber InFrameNumber);
	void AddMarkedFrame(FFrameNumber FrameNumber);
	
	/** Get the unqualified local time */
	FFrameTime GetLocalFrameTime() const;
	FQualifiedFrameTime GetLocalTime() const;
	FString GetFrameTimeText() const;
	uint32 GetLocalLoopIndex() const;
	void OnBeginScrubbing();
	void OnEndScrubbing();
	void SetGlobalTime(FFrameTime NewTime);
	void SetLocalTimeDirectly(FFrameTime NewTime);
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);
	UActActionSequence* GetActActionSequence() const;
	TSharedRef<FActActionSequenceEditor> GetActActionSequenceEditor() const;
protected:
	/**
	 * 对Editor的弱引用，调用编辑器资源和相关工具方法
	 */
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
	/**
	 * 所有已注册的CreateTrackEditor代理方法，在FActActionTrackEditorBase的子类中实现
	 */
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> TrackEditorDelegates;
	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;
	// UActActionSequence* ActActionSequencePtr;
	/**
	 * 当前Sequence时间轴的显示的范围，这里的单位是秒
	 */
	TRange<double> TargetViewRange;

	/** The last time range that was viewed */
	TRange<double> LastViewRange;
	/** Zoom smoothing curves */
	FCurveSequence ZoomAnimation;
	// FCurveHandle ZoomCurve;
	/**
	 * 当前的播放状态
	 */
	ActActionSequence::EPlaybackType PlaybackState;
	/**
	 * 当前的播放位置相关的数据结构
	 */
	ActActionSequence::FActActionPlaybackPosition PlayPosition;
	/** If set, pause playback on this frame */
	TOptional<FFrameTime> PauseOnFrame;
	/** When true, sequence will be forcefully evaluated on the next tick */
	bool bNeedsEvaluate;
	/** Local loop index at the time we began scrubbing */
	uint32 LocalLoopIndexOnBeginScrubbing;

	/** Local loop index to add for the purposes of displaying it in the UI */
	uint32 LocalLoopIndexOffsetDuringScrubbing;
	/** The amount of autoscroll pan offset that is currently being applied */
	TOptional<float> AutoscrollOffset;

	/** The amount of auto Scrub offset that is currently being applied */
	TOptional<float> AutoScrubOffset;
	TOptional<ActActionSequence::FActActionAutoScrubTarget> AutoScrubTarget;

	/** Time slider controller for this sequencer */
	TSharedPtr<FActActionTimeSliderController> TimeSliderController;
	
	/**
	 * 所有可见节点DisplayedRootNodes的父节点，
	 * Sequence中所有可见根节点都储存在NodeTree中作为子节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> TreeViewRoot;
	/**
	 * UMG Sequence main
	 */
	TSharedPtr<SActActionSequenceWidget> SequenceWidget;

public:
	ActActionSequence::EPlaybackType GetPlaybackStatus() const
	{
		return PlaybackState;
	}
	
	TSharedRef<SActActionSequenceWidget> GetSequenceWidget() const
	{
		return SequenceWidget.ToSharedRef();
	}

	TSharedRef<FActActionSequenceTreeViewNode> GetTreeViewRoot() const
	{
		return TreeViewRoot.ToSharedRef();
	}
};
