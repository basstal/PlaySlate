#pragma once


class SActImageTrackCarWidget;
class SActNotifiesPanelLaneWidget;
class SActNotifiesPanelEditorLaneWidget;

class SActTrackPanelNotifiesPanel : public SCompoundWidget, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActTrackPanelNotifiesPanel) {}
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

	void Construct(const FArguments& InArgs);

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
	void Update();
protected:
	/** Cached list of anim tracks for notify node drag drop */
	TArray<TSharedPtr<SActNotifiesPanelLaneWidget>> NotifyTracks;

	/** Cached list of Notify editor tracks */
	TArray<TSharedPtr<SActNotifiesPanelEditorLaneWidget>> NotifyEditorTracks;

	/** 所有已被添加的 TrackCar Widget */
	TArray<TSharedRef<SActImageTrackCarWidget>> ActImageTrackCarWidgets;
	
	TSharedPtr<SBorder> PanelArea;
	
	bool bIsSelecting;
	bool bIsUpdating;
};
