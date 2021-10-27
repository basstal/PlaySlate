#pragma once

#include "Common/NovaDataBinding.h"
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

	virtual ~SActSliderWidget() override;

	void Construct(const FArguments& InArgs, const TSharedRef<SGridPanel>& InParentGridPanel);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~End SWidget interface

	/**
	 * TODO:
	 */
	FFrameTime ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping = true) const;

	/**
	 * TODO:
	 */
	FFrameTime ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, FActActionScrubRangeToScreen RangeToScreen) const;

	/**
	 * Call this method when the user's interaction has changed the scrub position
	 *
	 * @param NewValue				Value resulting from the user's interaction
	 * @param bIsScrubbing			True if done via scrubbing, false if just releasing scrubbing
	 */
	void CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing);
	/**
	 * 设置Viewport的回放状态
	 *
	 * @param InPlaybackStatus 当前外部传入的播放状态
	 */
	void SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus);

	/**
	 * 时间轴拖拽器开始拖拽的回调
	 */
	void OnBeginScrubberMovement();

	/**
	 * 时间轴拖拽器结束拖拽的回调
	 */
	void OnEndScrubberMovement();

	/**
	 * 时间轴拖拽器位置改变的回调
	 */
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);

	/**
	 * Set a new range based on a min, max and an interpolation mode
	 * 
	 * @param InViewRange
	 */
	void OnViewRangeChanged(TSharedPtr<TRange<float>> InViewRange) const;

	/**
	 * Hit test the lower bound of a range
	 */
	bool HitTestRangeStart(const FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;

	/**
	 * Hit test the upper bound of a range
	 */
	bool HitTestRangeEnd(const FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;

	// /**
	//  * TODO:
	//  */
	// void SetPlaybackRangeStart(FFrameNumber NewStart) const;
	//
	// /**
	// * TODO:
	// */
	// void SetPlaybackRangeEnd(FFrameNumber NewEnd) const;

	// /**
	// * TODO:
	// */
	// void SetSelectionRangeStart(FFrameNumber NewStart) const;
	//
	// /**
	// * TODO:
	// */
	// void SetSelectionRangeEnd(FFrameNumber NewEnd) const;

	/**
	 * Zoom the range by a given delta.
	 * 
	 * @param InDelta		The total amount to zoom by (+ve = zoom out, -ve = zoom in)
	 * @param ZoomBias		Bias to apply to lower/upper extents of the range. (0 = lower, 0.5 = equal, 1 = upper)
	 */
	bool ZoomByDelta(float InDelta, float ZoomBias = 0.5f) const;

	/**
	 * Pan the range by a given delta
	 * 
	 * @param InDelta		The total amount to pan by (+ve = pan forwards in time, -ve = pan backwards in time)
	 */
	void PanByDelta(float InDelta) const;

	/**
	 * 绘制帧显示
	 *
	 * @param OutDrawElements 待绘制的元素列表
	 * @param ViewRange	当前绘制帧的显示范围
	 * @param RangeToScreen	当前显示范围换算成屏幕像素
	 * @param InArgs 其他参数
	 */
	static void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActActionScrubRangeToScreen& RangeToScreen, const FActActionDrawTickArgs& InArgs);

	/** 获得共享参数 */
	TSharedRef<FActEventTimelineArgs> GetActEventTimelineArgs() const;
	// /** 获得相关事件绑定 */
	// TSharedRef<FActEventTimelineEvents> GetActEventTimelineEvents() const;

protected:
	/** Total mouse delta during dragging **/
	float DistanceDragged;

	/**
	 * 当前鼠标的拖拽状态
	 */
	ENovaDragType MouseDragType;

	/** Mouse down position range */
	FVector2D MouseDownPosition[2];

	/** If mouse down was in time scrubbing region, only allow setting time when mouse is pressed down in the region */
	bool bMouseDownInRegion;

	/** If we are currently panning the panel */
	bool bPanning;

	/** Range stack */
	TArray<TRange<float>> ViewRangeStack;

	/** Geometry on mouse down */
	FGeometry MouseDownGeometry;

	/* If we should mirror the labels on the timeline */
	bool bMirrorLabels;

	/**
	* SectionOverlay的Controller，这个用来绘制TickLines
	*/
	TSharedPtr<SActImageThickLine> TickLinesSequenceSectionOverlayController;

	/**
	* SectionOverlay的Controller，这个用来绘制Scrub位置
	*/
	TSharedPtr<SActImageThickLine> ScrubPosSequenceSectionOverlayController;
	TSharedPtr<TDataBindingSP<TRange<float>>> ViewRangeDB;// ** 数据绑定

public:
	TSharedRef<SActImageThickLine> GetTickLinesSequenceSectionOverlayController() const
	{
		return TickLinesSequenceSectionOverlayController.ToSharedRef();
	}

	TSharedRef<SActImageThickLine> GetScrubPosSequenceSectionOverlayController() const
	{
		return ScrubPosSequenceSectionOverlayController.ToSharedRef();
	}
};
