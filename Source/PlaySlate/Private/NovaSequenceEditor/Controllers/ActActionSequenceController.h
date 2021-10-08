#pragma once

#include "Utils/ActActionPlaybackUtil.h"
#include "Utils/ActActionSequenceUtil.h"

#include "IContentBrowserSingleton.h"
class FActActionTimeSliderController;
class SActActionSequenceWidget;
class ASkeletalMeshActor;
class UActActionSequence;
class FActActionSequenceDisplayNode;
class SActActionViewportWidget;
class FActActionSequenceNodeTree;
/**
 * 整个Sequence的主要控制器
 * 对应的View模块为SActActionSequenceMain
 */
class FActActionSequenceController : public TSharedFromThis<FActActionSequenceController>, FTickableEditorObject
{
public:
	FActActionSequenceController(UActActionSequence* InActActionSequence);
	virtual ~FActActionSequenceController() override;
	
	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~End FTickableEditorObject interface

	void UpdateAnimInstance(float DeltaTime);
	void InitController(const TSharedRef<SWidget>& ViewWidget, const TArray<ActActionSequence::OnCreateTrackEditorDelegate>& TrackEditorDelegates, const TSharedRef<SActActionSequenceWidget>& InSequenceWidget);

	
	void UpdateTimeBases();
	AActor* SpawnActorInViewport(UClass* ActorType);
	/**
	 * ** FIX:Builds up the object bindings in sequencer's "Add Track" menu.
	 * 
	 * @param MenuBuilder 被添加内容的菜单
	 */
	void BuildAddObjectBindingsMenu(FMenuBuilder& MenuBuilder);
	/**
	 * ** FIX:Builds up the sequencer's "Add Track" menu.
	 *
	 * @param MenuBuilder 被添加内容的菜单
	 */
	void BuildAddTrackMenu(FMenuBuilder& MenuBuilder);

	/** @return The current view range */
	ActActionSequence::FActActionAnimatedRange GetViewRange() const;

	bool IsReadOnly() const;
	/**
	* Get the tick resolution of the currently focused sequence
	*/
	FFrameRate GetFocusedTickResolution() const;
	FFrameRate GetFocusedDisplayRate() const;
	/** Handles adding a new folder to the outliner tree. */
	void OnAddFolder();

	void RequestListRefresh();

	void AddRootNodes(TSharedPtr<FActActionSequenceDisplayNode> SequenceDisplayNode);
	ActActionSequence::FActActionAnimatedRange GetClampRange() const;
	TRange<FFrameNumber> GetPlaybackRange() const;
	ActActionSequence::EPlaybackType GetPlaybackStatus() const;
	TRange<FFrameNumber> GetSelectionRange() const;
	
	void AddAnimMontageTrack(UAnimMontage* AnimMontage);
	void SetPlaybackStatus(ActActionSequence::EPlaybackType InPlaybackStatus);
	void EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange, bool bHasJumped = false);
	void Pause();
	/** Stop the sequencer from auto-scrolling */
	void StopAutoscroll();
protected:
	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;
	UActActionSequence* ActActionSequencePtr;
	/** The time range target to be viewed */
	TRange<double> TargetViewRange;

	/** The last time range that was viewed */
	TRange<double> LastViewRange;
	/** Zoom smoothing curves */
	FCurveSequence ZoomAnimation;
	// FCurveHandle ZoomCurve;
	TSharedPtr<SActActionSequenceWidget> SequenceWidget;
	ActActionSequence::EPlaybackType PlaybackState;
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
	double PreviewScrubTime;
public:
	TSharedPtr<SActActionSequenceWidget> GetSequenceWidget() const
	{
		return SequenceWidget;
	}

	UActActionSequence* GetActActionSequencePtr() const
	{
		return ActActionSequencePtr;
	}

protected:
	TSharedPtr<SActActionViewportWidget> ActActionViewportWidget;

	ASkeletalMeshActor* PreviewActor;

	/**
	 * 左侧Track的所有可见节点都储存在NodeTree中
	 */
	TSharedPtr<FActActionSequenceNodeTree> NodeTree;

public:
	// FAssetPickerConfig InAssetPickerConfig;

	void SetNodeTree(const TSharedPtr<FActActionSequenceNodeTree>& InNodeTree)
	{
		this->NodeTree = InNodeTree;
	}

	TSharedRef<FActActionSequenceNodeTree> GetNodeTree() const
	{
		return NodeTree.ToSharedRef();
	}

	TSharedPtr<SActActionSequenceWidget> MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams);
	TSet<FFrameNumber> GetVerticalFrames() const;
	void SetMarkedFrame(int32 InMarkIndex, FFrameNumber InFrameNumber);
	void AddMarkedFrame(FFrameNumber FrameNumber);
	void SetPlaybackRange(TRange<FFrameNumber> Range);
	void InitAnimBlueprint(UAnimBlueprint* AnimBlueprint);
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
};
