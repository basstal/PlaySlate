#pragma once

#include "Common/NovaStruct.h"

class SActActionTimeSliderWidget;
class SActSliderViewRangeBarWidget;
class SActEventTimelineViewRangeWidget;
class SActImageThickLine;
class SGridPanel;

using namespace NovaStruct;


class SActSliderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActSliderWidget) { }
	SLATE_END_ARGS()

	SActSliderWidget();
	virtual ~SActSliderWidget() override;

	void Construct(const FArguments& InArgs, const TSharedRef<SGridPanel>& InParentGridPanel);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~End SWidget interface

	/**
	 * Call this method when the user's interaction has changed the scrub position
	 *
	 * @param NewValue Value resulting from the user's interaction
	 * @param bIsScrubbing True if done via scrubbing, false if just releasing scrubbing
	 */
	void CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing);

	/**
	 * 设置Viewport的回放状态
	 *
	 * @param InPlaybackStatus 待设置的播放状态
	 */
	void SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus);

	/**
	 * 时间轴刷开始拖拽的回调
	 */
	void OnBeginScrubberMovement();

	/**
	 * 时间轴刷结束拖拽的回调
	 */
	void OnEndScrubberMovement();

	/**
	 * 时间轴刷位置改变的回调
	 *
	 * @param NewScrubPosition 传入的 时间轴刷 位置
	 * @param bScrubbing 是否 时间轴刷 位置修改中
	 */
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);

	/**
	 * Zoom the range by a given delta.
	 * 
	 * @param InDelta The total amount to zoom by (+ve = zoom out, -ve = zoom in)
	 * @param ZoomBias Bias to apply to lower/upper extents of the range. (0 = lower, 0.5 = equal, 1 = upper)
	 */
	bool ZoomByDelta(float InDelta, float ZoomBias = 0.5f) const;

	/**
	 * Pan the range by a given delta
	 * 
	 * @param InDelta The total amount to pan by (+ve = pan forwards in time, -ve = pan backwards in time)
	 */
	void PanByDelta(float InDelta) const;

	/**
	 * 绘制帧显示
	 *
	 * @param OutDrawElements 待绘制的元素列表
	 * @param RangeToScreen	当前显示范围，以及换算成屏幕像素的相关方法
	 * @param InArgs 其他相关参数
	 */
	static void DrawTicks(
		FSlateWindowElementList& OutDrawElements,
		const FActSliderScrubRangeToScreen& RangeToScreen,
		const FActDrawTickArgs& InArgs);

protected:
	/**
	 * @return 获得 EventTimeline 共享参数，仅批量修改时用
	 */
	TSharedRef<FActEventTimelineArgs> GetActEventTimelineArgs() const;
	/** 鼠标拖拽的累计距离，Total mouse delta during dragging */
	float DistanceDragged;
	/** 当前鼠标的拖拽状态，是在拖拽时间平移还是改变时间范围 */
	ENovaDragType MouseDragType;
	/** 当前是否在拖拽平移的过程中，If we are currently panning the panel */
	bool bPanning;
	/** 鼠标按下的位置范围值，Mouse down position range */
	FVector2D MouseDownPosition[2];
	/** 给 DRAG_SETTING_RANGE Zoom 功能使用，Range stack */
	TArray<TRange<double>> ViewRangeZoomStack;
	/** 记录鼠标按下时传入的 Geometry，Geometry on mouse down */
	FGeometry MouseDownGeometry;
};
