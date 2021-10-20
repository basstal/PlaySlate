#pragma once

#include "Common/NovaStruct.h"

class SActActionTimeSliderWidget;

class SActActionTimeRangeSlider;

class SActActionTimeRange;

class FActEventTimelineImage;

/**
 * Sequence的时间滑块控制器，管理Sequence的时间轴相关数据
 * 对应的View模块为SActActionSequenceTimeSliderWidget
 */
class FActActionTimeSliderController : public TSharedFromThis<FActActionTimeSliderController>
{
	friend class SActActionTimeSliderWidget;

public:
	FActActionTimeSliderController(const TSharedRef<FActEventTimelineBrain>& InSequenceController);

	~FActActionTimeSliderController();

	/**
	* 构造TimeSlider的Widget
	*/
	void MakeTimeSliderWidget();

	/**
	 * TODO:
	 */
	FFrameTime ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping = true) const;

	/**
	 * TODO:
	 */
	FFrameTime ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen) const;

	/**
	 * Call this method when the user's interaction has changed the scrub position
	 *
	 * @param NewValue				Value resulting from the user's interaction
	 * @param bIsScrubbing			True if done via scrubbing, false if just releasing scrubbing
	 */
	void CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing) const;

	/**
	 * Set a new range based on a min, max and an interpolation mode
	 * 
	 * @param NewRangeMin		The new lower bound of the range
	 * @param NewRangeMax		The new upper bound of the range
	 * @param Interpolation		How to set the new range (either immediately, or animated)
	 */
	void SetViewRange(double NewRangeMin, double NewRangeMax, ENovaViewRangeInterpolation Interpolation) const;

	/**
	 * Hit test the lower bound of a range
	 */
	bool HitTestRangeStart(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;

	/**
	 * Hit test the upper bound of a range
	 */
	bool HitTestRangeEnd(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;

	/**
	 * TODO:
	 */
	void SetPlaybackRangeStart(FFrameNumber NewStart) const;

	/**
	* TODO:
	*/
	void SetPlaybackRangeEnd(FFrameNumber NewEnd) const;

	/**
	* TODO:
	*/
	void SetSelectionRangeStart(FFrameNumber NewStart) const;

	/**
	* TODO:
	*/
	void SetSelectionRangeEnd(FFrameNumber NewEnd) const;

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
	 * Widget的OnMouseButtonDown回调
	 * 
	 * @param MyGeometry 当前点击的Geometry
	 * @param MouseEvent 当前响应的鼠标事件
	 */
	FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	/**
	 * Widget的OnMouseButtonUp回调
	 * 
	 * @param MyGeometry 当前点击的Geometry
	 * @param MouseEvent 当前响应的鼠标事件
	 */
	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	/**
	 * Widget的OnMouseMove回调
	 * 
	 * @param MyGeometry 当前点击的Geometry
	 * @param MouseEvent 当前响应的鼠标事件
	 */
	FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);

	/**
	 * Widget的OnMouseWheel回调
	 * 
	 * @param MyGeometry 当前点击的Geometry
	 * @param MouseEvent 当前响应的鼠标事件
	 */
	FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;

	/**
	 * @return 获得TimeSlider相关参数
	 */
	ActActionSequence::FActActionTimeSliderArgs& GetTimeSliderArgs() const;

	/** 绘制子Sequence范围，TODO:当前可能不需要 */
	int32 DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;

	/** 绘制Playback范围，TODO:当前可能不需要 */
	int32 DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;

	/**
	 * 绘制帧显示
	 *
	 * @param OutDrawElements 待绘制的元素列表
	 * @param ViewRange	当前绘制帧的显示范围
	 * @param RangeToScreen	当前显示范围换算成屏幕像素
	 * @param InArgs 其他参数
	 */
	void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionDrawTickArgs& InArgs) const;

protected:
	/**
	 * 隶属的Controller
	 */
	TWeakPtr<FActEventTimelineBrain> ActActionSequenceController;

	/** 对应控制的Widget */
	TSharedPtr<SActActionTimeSliderWidget> ActActionTimeSliderWidget;

	/** TimeRange Widget */
	TSharedPtr<SActActionTimeRange> ActActionTimeRange;

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
	TArray<TRange<double>> ViewRangeStack;

	/** Geometry on mouse down */
	FGeometry MouseDownGeometry;

	/* If we should mirror the labels on the timeline */
	bool bMirrorLabels;

	/**
	* SectionOverlay的Controller，这个用来绘制TickLines
	*/
	TSharedPtr<FActEventTimelineImage> TickLinesSequenceSectionOverlayController;

	/**
	* SectionOverlay的Controller，这个用来绘制Scrub位置
	*/
	TSharedPtr<FActEventTimelineImage> ScrubPosSequenceSectionOverlayController;

public:
	TSharedRef<SActActionTimeSliderWidget> GetActActionTimeSliderWidget() const
	{
		return ActActionTimeSliderWidget.ToSharedRef();
	}

	TSharedRef<SActActionTimeRange> GetActActionTimeRange() const
	{
		return ActActionTimeRange.ToSharedRef();
	}

	TSharedRef<FActEventTimelineImage> GetTickLinesSequenceSectionOverlayController() const
	{
		return TickLinesSequenceSectionOverlayController.ToSharedRef();
	}

	TSharedRef<FActEventTimelineImage> GetScrubPosSequenceSectionOverlayController() const
	{
		return ScrubPosSequenceSectionOverlayController.ToSharedRef();
	}
};
