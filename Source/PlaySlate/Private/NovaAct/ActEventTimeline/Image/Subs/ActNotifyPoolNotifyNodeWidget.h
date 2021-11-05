#pragma once
#include "Common/NovaEnum.h"
#include "Common/NovaDelegate.h"

class SActNotifyPoolLaneWidget;

using namespace NovaDelegate;

class SActNotifyPoolNotifyNodeWidget : public SLeafWidget
{
	friend class FActTrackAreaSlotDragDrop;
public:
	SLATE_BEGIN_ARGS(SActNotifyPoolNotifyNodeWidget) { }
		SLATE_ARGUMENT(FAnimNotifyEvent*, AnimNotifyEvent)
		// SLATE_EVENT(OnNotifyNodeDragStartedDelegate, OnNodeDragStarted)
		// SLATE_EVENT(OnNotifyStateHandleBeingDraggedDelegate, OnNotifyStateHandleBeingDragged)
		SLATE_EVENT(FSimpleDelegate, OnUpdatePanel)
		SLATE_EVENT(OnPanTrackRequestDelegate, PanTrackRequest)
		SLATE_EVENT(FSimpleDelegate, OnSelectionChanged)
		SLATE_EVENT(OnSnapPositionDelegate, OnSnapPosition)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SActNotifyPoolLaneWidget>& InActNotifiesPanelLaneWidget);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FCursorReply OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual void OnFocusLost(const FFocusEvent& InFocusEvent) override;
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	//~End SWidget interface

	/** 获得描述文本 */
	FText GetNotifyText() const;
	// /** 获得颜色 */
	// FLinearColor GetNotifyColor() const;
	/**
	 * 更新大小和位置信息
	 *
	 * @param AllottedGeometrySizeX Lane X 大小
	 */
	void UpdateSizeAndPosition(float AllottedGeometrySizeX);
	/** 绘制拖拽按钮 */
	void DrawScrubHandle(float ScrubHandleCentre,
	                     FSlateWindowElementList& OutDrawElements,
	                     int32 ScrubHandleID,
	                     const FGeometry& AllottedGeometry,
	                     const FSlateRect& MyCullingRect,
	                     FLinearColor NodeColour) const;
	/** 拖拽按钮的Offset */
	void DrawHandleOffset(const float& Offset,
	                      const float& HandleCentre,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 MarkerLayer,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyCullingRect,
	                      FLinearColor NodeColor) const;
	/** TODO: */
	ENovaNotifyStateHandleHit DurationHandleHitTest(const FVector2D& CursorTrackPosition) const;
	/** TODO: */
	FVector2D GetWidgetPosition() const;
	/** TODO: */
	float HandleOverflowPan(const FVector2D& ScreenCursorPos, float TrackScreenSpaceXPosition, float TrackScreenSpaceMin, float TrackScreenSpaceMax);

	/** TODO: */
	FVector2D GetNotifyPositionOffset() const;
	FVector2D GetNotifyPosition() const;
	/** @return 是否在被选中状态 */
	bool IsSelected() const;
protected:
	

	/** 当前拖拽的状态 */
	ENovaNotifyStateHandleHit CurrentDragHandle;

	/** 是否往右 */
	bool bDrawTooltipToRight;


	/** Delegate that is called when the user initiates dragging */
	// ActActionSequence::OnNotifyNodeDragStartedDelegate OnNodeDragStarted;
	/** Delegate that is called when a notify state handle is being dragged */
	// OnNotifyStateHandleBeingDraggedDelegate OnNotifyStateHandleBeingDragged;
	/** Delegate to redraw the notify panel */
	FSimpleDelegate OnUpdatePanel;
	/** Delegate to pan the track, needed if the markers are dragged out of the track */
	OnPanTrackRequestDelegate PanTrackRequest;
	/** Delegate to signal selection changing */
	FSimpleDelegate OnSelectionChanged;
	/** Delegate used to snap when dragging */
	OnSnapPositionDelegate OnSnapPosition;
	// /** 缓存的 Lane 已分配 Widget 大小 */
	// FVector2D CachedLaneWidgetAllottedGeometrySize;
	/** TODO: */
	float NotifyTimePositionX;

	/** TODO: */
	FVector2D TextSize;
	/** TODO: */
	FVector2D BranchingPointIconSize;

	/** TODO: */
	float WidgetX;
	/** TODO: */
	float NotifyScrubHandleCentre;

	// /** Cached owning track geometry */
	// FGeometry CachedLaneGeometry;

	/** AnimNotifyEvent 决定的 NotifyNode 缓存名称 */
	FName CachedNotifyName;
	// Notify 颜色
	FLinearColor NotifyEditorColor;
	// Index for undo transactions for dragging, as a check to make sure it's active
	int32 DragMarkerTransactionIdx;
public:
	/** 对 Parent Widget 弱引用 */
	TWeakPtr<SActNotifyPoolLaneWidget> WeakActNotifyPoolLaneWidget;
	/** 是否正在拖拽 */
	bool bBeingDragged;

	/** Tick AllottedGeometry.AbsolutePosition */
	FVector2D ScreenPosition;
	/** Notify 持续时间 换算成屏幕像素距离 */
	float DurationSizeX;

	/** DragDrop Snap 使用的 Time */
	float LastSnappedTime;

	/** 当前节点的大小 */
	FVector2D WidgetSize;
	/** Last position the user clicked in the widget */
	FVector2D LastMouseDownPosition;
	// Animation Notify 相关数据结构
	FAnimNotifyEvent* AnimNotifyEvent;
public:
	const FVector2D AlignmentMarkerSize = FVector2D(10.f, 20.f);
	const FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 10);
};
