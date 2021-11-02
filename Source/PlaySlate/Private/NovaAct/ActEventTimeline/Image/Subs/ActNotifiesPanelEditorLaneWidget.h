#pragma once
#include "ActNotifiesPanelLaneWidget.h"

class SActNotifiesPanelEditorLaneWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActNotifiesPanelEditorLaneWidget)
			: _TrackIndex(INDEX_NONE) {}

		// 		, _AnimNotifyPanel()
		// 		, _Sequence()
		// 		, _WidgetWidth()
		// 		, _ViewInputMin()
		// 		, _ViewInputMax()
		// 		, _OnSelectionChanged()
		// 		, _OnUpdatePanel()
		// 		, _OnDeleteNotify()
		// 		, _OnDeselectAllNotifies()
		// 		, _OnCopyNodes()
		// 		, _OnSetInputViewRange()
		// {}
		SLATE_ARGUMENT(int32, TrackIndex)
		// SLATE_ARGUMENT( TSharedPtr<SAnimNotifyPanel>, AnimNotifyPanel)
		// SLATE_ARGUMENT( class UAnimSequenceBase*, Sequence )
		// SLATE_ARGUMENT( float, WidgetWidth )
		// SLATE_ATTRIBUTE( float, ViewInputMin )
		// SLATE_ATTRIBUTE( float, ViewInputMax )
		// SLATE_EVENT( FOnSnapPosition, OnSnapPosition )
		// SLATE_ATTRIBUTE( EVisibility, NotifyTimingNodeVisibility )
		// SLATE_ATTRIBUTE( EVisibility, BranchingPointTimingNodeVisibility )
		// SLATE_EVENT( FOnTrackSelectionChanged, OnSelectionChanged)
		// SLATE_EVENT( FOnGetScrubValue, OnGetScrubValue )
		// SLATE_EVENT( FOnGetDraggedNodePos, OnGetDraggedNodePos )
		// SLATE_EVENT( FOnUpdatePanel, OnUpdatePanel )
		// SLATE_EVENT( FOnGetBlueprintNotifyData, OnGetNotifyBlueprintData )
		// SLATE_EVENT( FOnGetBlueprintNotifyData, OnGetNotifyStateBlueprintData )
		// SLATE_EVENT( FOnGetNativeNotifyClasses, OnGetNotifyNativeClasses )
		// SLATE_EVENT( FOnGetNativeNotifyClasses, OnGetNotifyStateNativeClasses )
		// SLATE_EVENT( FOnNotifyNodesDragStarted, OnNodeDragStarted )
		// SLATE_EVENT( FOnNotifyStateHandleBeingDragged, OnNotifyStateHandleBeingDragged)
		// SLATE_EVENT( FRefreshOffsetsRequest, OnRequestRefreshOffsets )
		// SLATE_EVENT( FDeleteNotify, OnDeleteNotify )
		// SLATE_EVENT( FDeselectAllNotifies, OnDeselectAllNotifies)
		// SLATE_EVENT( FCopyNodes, OnCopyNodes )
		// SLATE_EVENT( FPasteNodes, OnPasteNodes )
		// SLATE_EVENT( FOnSetInputViewRange, OnSetInputViewRange )
		// SLATE_EVENT( FOnGetTimingNodeVisibility, OnGetTimingNodeVisibility )
		// SLATE_EVENT(FOnInvokeTab, OnInvokeTab)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	TSharedRef<SActNotifiesPanelLaneWidget> GetNotifyTrack() const;

protected:
	/** Index of Track in Sequence **/
	int32 TrackIndex;

	/** Anim Sequence **/
	// UAnimSequenceBase* Sequence;

	/** Pointer to actual anim notify track */
	TSharedPtr<SActNotifiesPanelLaneWidget> NotifyTrack;

	/** Pointer to notify panel for drawing*/
	// TWeakPtr<SAnimNotifyPanel>			AnimPanelPtr;
};
