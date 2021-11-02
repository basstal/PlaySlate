#pragma once

class SActActionSequenceNotifyNode;

class SActNotifiesPanelLaneWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActNotifiesPanelLaneWidget)
			: _TrackIndex(INDEX_NONE) {}

		// : _Sequence(NULL)
		// , _ViewInputMin()
		// , _ViewInputMax()

		// , _TrackColor(FLinearColor::White)
		// , _OnSelectionChanged()
		// , _OnUpdatePanel()
		// , _OnGetNotifyBlueprintData()
		// , _OnGetNotifyStateBlueprintData()
		// , _OnGetNotifyNativeClasses()
		// , _OnGetNotifyStateNativeClasses()
		// , _OnGetScrubValue()
		// , _OnGetDraggedNodePos()
		// , _OnNodeDragStarted()
		// , _OnNotifyStateHandleBeingDragged()
		// , _OnRequestTrackPan()
		// , _OnRequestOffsetRefresh()
		// , _OnDeleteNotify()
		// , _OnGetIsAnimNotifySelectionValidForReplacement()
		// , _OnReplaceSelectedWithNotify()
		// , _OnReplaceSelectedWithBlueprintNotify()
		// , _OnDeselectAllNotifies()
		// , _OnCopyNodes()
		// , _OnPasteNodes()
		// , _OnSetInputViewRange()
		// {}
		//
		// SLATE_ARGUMENT( class UAnimSequenceBase*, Sequence )
		// SLATE_ARGUMENT( TArray<FAnimNotifyEvent *>, AnimNotifies )
		// SLATE_ARGUMENT( TArray<FAnimSyncMarker *>, AnimSyncMarkers)
		// SLATE_ATTRIBUTE( float, ViewInputMin )
		// SLATE_ATTRIBUTE( float, ViewInputMax )
		// SLATE_EVENT( FOnSnapPosition, OnSnapPosition )
		SLATE_ARGUMENT(int32, TrackIndex)
		// SLATE_ARGUMENT( FLinearColor, TrackColor )
		// SLATE_ATTRIBUTE(EVisibility, QueuedNotifyTimingNodeVisibility)
		// SLATE_ATTRIBUTE(EVisibility, BranchingPointTimingNodeVisibility)
		// SLATE_EVENT(FOnTrackSelectionChanged, OnSelectionChanged)
		// SLATE_EVENT( FOnUpdatePanel, OnUpdatePanel )
		// SLATE_EVENT( FOnGetBlueprintNotifyData, OnGetNotifyBlueprintData )
		// SLATE_EVENT( FOnGetBlueprintNotifyData, OnGetNotifyStateBlueprintData )
		// SLATE_EVENT( FOnGetNativeNotifyClasses, OnGetNotifyNativeClasses )
		// SLATE_EVENT( FOnGetNativeNotifyClasses, OnGetNotifyStateNativeClasses )
		// SLATE_EVENT( FOnGetScrubValue, OnGetScrubValue )
		// SLATE_EVENT( FOnGetDraggedNodePos, OnGetDraggedNodePos )
		// SLATE_EVENT( FOnNotifyNodesDragStarted, OnNodeDragStarted )
		// SLATE_EVENT( FOnNotifyStateHandleBeingDragged, OnNotifyStateHandleBeingDragged)
		// SLATE_EVENT( FPanTrackRequest, OnRequestTrackPan )
		// SLATE_EVENT( FRefreshOffsetsRequest, OnRequestOffsetRefresh )
		// SLATE_EVENT( FDeleteNotify, OnDeleteNotify )
		// SLATE_EVENT( FOnGetIsAnimNotifySelectionValidForReplacement, OnGetIsAnimNotifySelectionValidForReplacement)
		// SLATE_EVENT( FReplaceWithNotify, OnReplaceSelectedWithNotify )
		// SLATE_EVENT( FReplaceWithBlueprintNotify, OnReplaceSelectedWithBlueprintNotify)
		// SLATE_EVENT( FDeselectAllNotifies, OnDeselectAllNotifies)
		// SLATE_EVENT( FCopyNodes, OnCopyNodes )
		// SLATE_EVENT(FPasteNodes, OnPasteNodes)
		// SLATE_EVENT( FOnSetInputViewRange, OnSetInputViewRange )
		// SLATE_EVENT( FOnGetTimingNodeVisibility, OnGetTimingNodeVisibility )
		// SLATE_EVENT(FOnInvokeTab, OnInvokeTab)
		// SLATE_ARGUMENT(TSharedPtr<FUICommandList>, CommandList)
	SLATE_END_ARGS()
	/** Type used for list widget of tracks */
	void Construct(const FArguments& InArgs);
	void Update();
	FMargin GetNotifyTrackPadding(int32 NotifyIndex) const;
	FText GetNodeTooltip();
	float GetPlayLength();
	FName GetName();
	FLinearColor GetEditorColor();
	bool IsBranchingPoint();

protected:
	TSharedPtr<SBorder> TrackBorder;

	/** Cache the SOverlay used to store all this tracks nodes */
	TSharedPtr<SOverlay> NodeSlots;

	int32 TrackIndex;

	/** Cached for drag drop handling code */
	FGeometry CachedGeometry;
	
	TArray<TSharedPtr<SActActionSequenceNotifyNode>> NotifyNodes;
};
