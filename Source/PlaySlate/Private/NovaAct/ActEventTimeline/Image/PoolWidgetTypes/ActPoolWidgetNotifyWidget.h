﻿#pragma once
#include "StatusBarSubsystem.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActNotifyPoolNotifyNodeWidget.h"

class SActImageTrackCarWidget;
class SActNotifyPoolLaneWidget;
class SActNotifyPoolEditorLaneWidget;
class IActImageTrackBase;
class FActImageTrackNotify;


class SActPoolWidgetNotifyWidget : public SCompoundWidget, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActPoolWidgetNotifyWidget) {}
		// 	: _Sequence()
		// 	, _CurrentPosition()
		// 	, _ViewInputMin()
		// 	, _ViewInputMax()
		// 	, _InputMin()
		// 	, _InputMax()
		// 	, _OnSetInputViewRange()
		// 	, _OnSelectionChanged()
		// 	, _OnGetScrubValue()
		// 	, _OnRequestRefreshOffsets()
		// {}

		// SLATE_ARGUMENT( class UAnimSequenceBase*, Sequence)
		// SLATE_ARGUMENT( float, WidgetWidth )
		// SLATE_ATTRIBUTE( float, CurrentPosition )
		// SLATE_ATTRIBUTE( float, ViewInputMin )
		// SLATE_ATTRIBUTE( float, ViewInputMax )
		// SLATE_ATTRIBUTE( float, InputMin )
		// SLATE_ATTRIBUTE( float, InputMax )
		// SLATE_EVENT( FOnSetInputViewRange, OnSetInputViewRange )
		// SLATE_EVENT( FOnSelectionChanged, OnSelectionChanged )
		// SLATE_EVENT( FOnGetScrubValue, OnGetScrubValue )
		// SLATE_EVENT( FRefreshOffsetsRequest, OnRequestRefreshOffsets )
		// SLATE_EVENT( FOnGetTimingNodeVisibility, OnGetTimingNodeVisibility )
		// SLATE_EVENT( FOnInvokeTab, OnInvokeTab )
		// SLATE_EVENT( FSimpleDelegate, OnNotifiesChanged )
		// SLATE_EVENT( FOnSnapPosition, OnSnapPosition )
		// SLATE_EVENT( FOnNotifyStateHandleBeingDragged, OnNotifyStateHandleBeingDragged)
		// SLATE_EVENT( FOnNotifyNodesBeingDragged, OnNotifyNodesBeingDragged)

	SLATE_END_ARGS()

	virtual ~SActPoolWidgetNotifyWidget() override;

	void Construct(const FArguments& InArgs, const TSharedRef<FActImageTrackNotify>& InActImageTrackNotify);

	//~Begin SCompoundWidget interface
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~End SCompoundWidget interface

	/** Handler for delete command */
	void OnDeletePressed();


	void DeleteSelectedNodeObjects();

	// this just refresh notify tracks - UI purpose only
	// do not call this from here. This gets called by asset. 
	void RefreshNotifyTracks();
	/**
	 * Called when a track changes it's selection; iterates all tracks collecting selected items
	 */
	void OnTrackSelectionChanged();

	/**
	 * 刷新 Lane 显示的内容
	 *
	 * @param InActImageTrack
	 */
	void OnLaneContentRefresh(TSharedPtr<IActImageTrackBase> InActImageTrack);
	/**
	 * Handler that is called when the user starts dragging a node
	 * @param InLane
	 * @param InNotifyNode
	 * @param InMouseEvent
	 * @param bDragOnMarker
	 */
	FReply OnNotifyNodeDragStarted(const TSharedRef<SActNotifyPoolLaneWidget>& InLane,
	                               const TSharedRef<SActNotifyPoolNotifyNodeWidget>& InNotifyNode,
	                               const FPointerEvent& InMouseEvent,
	                               const bool bDragOnMarker);

	/**
	 * 从指定的 Lane 中设置选中指定的 NotifyNode
	 * @param NotifyNode
	 * @param Append 是否为添加状态，否则会先取消所有已选中的 NotifyNode
	 */
	void SelectNotifyNode(const TSharedRef<SActNotifyPoolNotifyNodeWidget>& NotifyNode,
	                      bool Append,
	                      bool bUpdateSelection);
	/**
	 * 取消所有 Lane 的所有 NotifyNode 的选中状态
	 */
	void DeselectAllNotifies();

	/**
	 */
	void OnNotifyNodesBeingDraggedStatusBarMessage();
	
protected:
	TSharedPtr<FActImageTrackNotify> ActImageTrackNotify;

	

	/** Cached list of Notify editor tracks */
	TArray<TSharedPtr<SActNotifyPoolEditorLaneWidget>> NotifyEditorTracks;

	/** 所有已被添加的 TrackCar Widget */
	TArray<TSharedRef<SActImageTrackCarWidget>> ActImageTrackCarWidgets;

	TSharedPtr<SBorder> PanelArea;

	/** Recursion guard for selection */
	bool bIsSelecting;
	bool bIsUpdating;
	/** Store the position of a currently dragged node for display across tracks */
	float CurrentDragXPosition;
	/** Handle to status bar message */
	FStatusBarMessageHandle StatusBarMessageHandle;
public:
	/** Cached list of anim tracks for notify node drag drop */
	TArray<TSharedPtr<SActNotifyPoolLaneWidget>> NotifyLanes;
	
	TSet<TSharedRef<SWidget const>> SelectedNotifyNodes;
};
