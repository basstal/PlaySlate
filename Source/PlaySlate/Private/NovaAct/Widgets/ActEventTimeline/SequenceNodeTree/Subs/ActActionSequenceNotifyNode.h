#pragma once
#include "Common/NovaEnum.h"
#include "Common/NovaDelegate.h"

class FActActionTrackAreaSlot;

class SActActionSequenceNotifyNode : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceNotifyNode)
		{
		}

		SLATE_EVENT(ActActionSequence::OnNotifyNodeDragStartedDelegate, OnNodeDragStarted)
		SLATE_EVENT(ActActionSequence::OnNotifyStateHandleBeingDraggedDelegate, OnNotifyStateHandleBeingDragged)
		SLATE_EVENT(FSimpleDelegate, OnUpdatePanel)
		SLATE_EVENT(ActActionSequence::OnPanTrackRequestDelegate, PanTrackRequest)
		SLATE_EVENT(FSimpleDelegate, OnSelectionChanged)
		SLATE_EVENT(ActActionSequence::OnSnapPositionDelegate, OnSnapPosition)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionTrackAreaSlot>& InTrackAreaSlot);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	//~End SWidget interface

	/** 获得描述文本 */
	FText GetNotifyText() const;
	/** 获得颜色 */
	FLinearColor GetNotifyColor() const;
	/** 更新大小和位置信息 */
	void UpdateSizeAndPosition(const FGeometry& AllottedGeometry);
	/** 绘制拖拽按钮 */
	void DrawScrubHandle(float ScrubHandleCentre, FSlateWindowElementList& OutDrawElements, int32 ScrubHandleID, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FLinearColor NodeColour) const;
	/** 拖拽按钮的Offset */
	void DrawHandleOffset(const float& Offset, const float& HandleCentre, FSlateWindowElementList& OutDrawElements, int32 MarkerLayer, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FLinearColor NodeColor) const;
	/** TODO: */
	ENovaNotifyStateHandleHit DurationHandleHitTest(const FVector2D& CursorTrackPosition) const;
	/** TODO: */
	FVector2D GetWidgetPosition() const;
protected:
	/** Controller 弱引用 */
	TWeakPtr<FActActionTrackAreaSlot> ActActionTrackAreaSlot;

	/** 是否正在拖拽 */
	bool bBeingDragged;

	/** 当前拖拽的状态 */
	ENovaNotifyStateHandleHit CurrentDragHandle;

	/** 是否往右 */
	bool bDrawTooltipToRight;

	/** 是否选中状态 */
	bool bSelected;

	/** Delegate that is called when the user initiates dragging */
	ActActionSequence::OnNotifyNodeDragStartedDelegate OnNodeDragStarted;
	/** Delegate that is called when a notify state handle is being dragged */
	ActActionSequence::OnNotifyStateHandleBeingDraggedDelegate OnNotifyStateHandleBeingDragged;
	/** Delegate to redraw the notify panel */
	FSimpleDelegate OnUpdatePanel;
	/** Delegate to pan the track, needed if the markers are dragged out of the track */
	ActActionSequence::OnPanTrackRequestDelegate PanTrackRequest;
	/** Delegate to signal selection changing */
	FSimpleDelegate OnSelectionChanged;
	/** Delegate used to snap when dragging */
	ActActionSequence::OnSnapPositionDelegate OnSnapPosition;
	/** 缓存的已分配大小 */
	FVector2D CachedAllottedGeometrySize;
	/** TODO: */
	float NotifyTimePositionX;
	/** TODO: */
	float NotifyDurationSizeX;
	/** TODO: */
	FVector2D TextSize;
	/** TODO: */
	float LabelWidth;
	/** TODO: */
	FVector2D BranchingPointIconSize;
	/** TODO: */
	FVector2D WidgetSize;
	/** TODO: */
	float WidgetX;
	/** TODO: */
	float NotifyScrubHandleCentre;
	/** Last position the user clicked in the widget */
	FVector2D LastMouseDownPosition;
public:
	bool GetBeingDragged() const
	{
		return bBeingDragged;
	}

public:
	const FVector2D ScrubHandleSize = FVector2D(12.0f, 12.0f);
	const FVector2D TextBorderSize = FVector2D(1.f, 1.f);
	const FVector2D AlignmentMarkerSize = FVector2D(10.f, 20.f);
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 10);
};
