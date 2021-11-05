#pragma once
#include "SCurveEditor.h"

class SActNotifyPoolNotifyNodeWidget;

class SActNotifyPoolLaneWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActNotifyPoolLaneWidget)
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

	//~Begin SCompoundWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~End SCompoundWidget interface

	void Update();
	FMargin GetNotifyTrackPadding(int32 NotifyIndex) const;
	FText GetNodeTooltip();
	float GetPlayLength();
	FName GetName();
	FLinearColor GetEditorColor();
	bool IsBranchingPoint();
	// /**
	//  * Deselects all currently selected notify nodes
	//  * @param bUpdateSelectionSet Whether we should report a selection change to the panel TODO:remove?
	//  */
	// void DeselectAllNotifyNodes(bool bUpdateSelectionSet);

	/**
	 * 从 NodeSlots 中删除已选中的 NotifyNode，并且将它们添加到 DragNodes中
	 * @param DragNodes 被删除的 NotifyNode
	 */
	void DisconnectSelectedNodesForDrag(TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>>& DragNodes);


protected:
	TSharedPtr<SBorder> TrackBorder;

	/** Cache the SOverlay used to store all this tracks nodes */
	TSharedPtr<SOverlay> NodeSlots;


	/** Cached for drag drop handling code */
	// FGeometry CachedGeometry;

public:
	int32 TrackIndex;
	// 缓存的 TrackScale 结构
	TSharedPtr<FTrackScaleInfo> CachedScaleInfo;

	// 缓存的 带缩放的 多边形大小
	FVector2D CachedAllottedGeometrySizeScaled;
	// /** Nodes that are currently selected */
	// TArray<int32> SelectedNodeIndices;
	// 在这个 Lane 上的所有 NotifyNode
	TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>> NotifyNodes;
};
