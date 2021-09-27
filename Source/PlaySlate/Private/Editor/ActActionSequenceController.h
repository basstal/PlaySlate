#pragma once

#include "ActActionSequenceDisplayNode.h"
#include "IContentBrowserSingleton.h"
#include "Assets/ActActionSequence.h"
#include "Assets/ActActionSequenceNodeTree.h"
#include "Assets/Tracks/ActActionTrackEditorBase.h"
#include "SWidget/ActActionViewportWidget.h"
#include "Utils/ActActionSequenceUtil.h"
class SActActionSequenceWidget;
class ASkeletalMeshActor;
/**
 * 整个Sequence的主要控制器
 * 对应的View模块为SActActionSequenceMain
 */
class FActActionSequenceController : public TSharedFromThis<FActActionSequenceController>
{
public:
	FActActionSequenceController(UActActionSequence* InActActionSequence);
	~FActActionSequenceController();
	AActor* SpawnActorInViewport(UClass* ActorType);


	void InitController(const TSharedRef<SWidget>& ViewWidget, const TArray<ActActionSequence::OnCreateTrackEditorDelegate>& TrackEditorDelegates, const TSharedRef<SActActionSequenceWidget>& InSequenceWidget);
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

	void AssignAsEditAnim(ASkeletalMeshActor* PreviewActor);

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
	FCurveHandle ZoomCurve;
	TSharedPtr<SActActionSequenceWidget> SequenceWidget;
	ActActionSequence::EPlaybackType PlaybackState;
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
};
