#pragma once

#include "Utils/ActActionSequenceUtil.h"

/**
 * Sequence的时间滑块控制器，管理Sequence的时间轴相关数据
 * 对应的View模块为SActActionSequenceTimeSliderWidget
 */
class FActActionTimeSliderController : public TSharedFromThis<FActActionTimeSliderController>
{
protected:
	/**
	 * 控制的Sequence对象
	 */
	TWeakPtr<FActActionSequenceController> Sequence;
	/**
	 * 控制器相关参数
	 */
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs;
	/** TRANS_EN:Numeric type interface used for converting parsing and generating strings from numbers */
	// TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
	/** TRANS_EN:Scrub style provided on construction */
	ActActionSequence::ESequencerScrubberStyle ScrubStyle;

	/** Total mouse delta during dragging **/
	float DistanceDragged;

	ActActionSequence::EDragType MouseDragType;

	/** Mouse down position range */
	FVector2D MouseDownPosition[2];

	/** Geometry on mouse down */
	FGeometry MouseDownGeometry;

	/** If mouse down was in time scrubbing region, only allow setting time when mouse is pressed down in the region */
	bool bMouseDownInRegion;

	/** Index of mark being edited */
	int32 DragMarkIndex;
	/** If we are currently panning the panel */
	bool bPanning;
	//
	// /** When > 0, we should not show context menus */
	// int32 ContextMenuSuppression;
	/** Range stack */
	TArray<TRange<double>> ViewRangeStack;
public:
	FActActionTimeSliderController(const ActActionSequence::FActActionTimeSliderArgs& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController);
	~FActActionTimeSliderController();
	
	ActActionSequence::ESequencerScrubberStyle GetScrubStyle() const
	{
		return ScrubStyle;
	}

	// TSharedRef<INumericTypeInterface<double>> GetNumericTypeInterface() const
	// {
	// return TimeSliderArgs.NumericTypeInterface.ToSharedRef()
	// }

	ActActionSequence::FActActionTimeSliderArgs GetTimeSliderArgs() const
	{
		return TimeSliderArgs;
	}

	TWeakPtr<FActActionSequenceController> GetSequence() const
	{
		return Sequence;
	}

	/**
	* TRANS_EN:Compute a major grid interval and number of minor divisions to display
	*/
	bool GetGridMetrics(float PhysicalWidth, double InViewStart, double InViewEnd, double& OutMajorInterval, int32& OutMinorDivisions) const;
	/**
	* TRANS_EN:Get the pixel metrics of the Scrubber
	* @param ScrubTime			The qualified time of the scrubber
	* @param RangeToScreen		Range to screen helper
	* @param DilationPixels		Number of pixels to dilate the handle by
	* @return FActActionScrubberMetrics
	*/
	ActActionSequence::FActActionScrubberMetrics GetScrubPixelMetrics(const FQualifiedFrameTime& ScrubTime, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, float DilationPixels = 0.0f) const;
	/**
	 * TODO:Comments
	 */
	FFrameTime ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping = true) const;
	/**
	* Draws major tick lines in the section view                                                              
	*/
	int32 OnPaintViewArea(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, bool bEnabled, const ActActionSequence::FActActionPaintViewAreaArgs& Args) const;
	int32 DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	int32 DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	int32 DrawSelectionRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, ActActionSequence::FActActionDrawTickArgs& InArgs) const;
	int32 DrawMarkedFrames(const FGeometry& AllottedGeometry, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ESlateDrawEffect& DrawEffects, bool bDrawLabels) const;
	int32 DrawVerticalFrames(const FGeometry& AllottedGeometry, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ESlateDrawEffect& DrawEffects) const;

	FFrameTime ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen) const;
	/**
	* Call this method when the user's interaction has changed the scrub position
	*
	* @param NewValue				Value resulting from the user's interaction
	* @param bIsScrubbing			True if done via scrubbing, false if just releasing scrubbing
	*/
	void CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing);
	/**
	* Clamp the given range to the clamp range 
	*
	* @param NewRangeMin		The new lower bound of the range
	* @param NewRangeMax		The new upper bound of the range
	*/
	void ClampViewRange(double& NewRangeMin, double& NewRangeMax);
	/**
	* Set a new clamp range based on a min, max
	* 
	* @param NewRangeMin		The new lower bound of the clamp range
	* @param NewRangeMax		The new upper bound of the clamp range
	*/
	void SetClampRange(double NewRangeMin, double NewRangeMax);
	/**
	* Set a new range based on a min, max and an interpolation mode
	* 
	* @param NewRangeMin		The new lower bound of the range
	* @param NewRangeMax		The new upper bound of the range
	* @param Interpolation		How to set the new range (either immediately, or animated)
	*/
	void SetViewRange(double NewRangeMin, double NewRangeMax, ActActionSequence::EActActionViewRangeInterpolation Interpolation);

	/**
	* Hit test the lower bound of a range
	*/
	bool HitTestRangeStart(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;
	/**
	* Hit test the upper bound of a range
	*/
	bool HitTestRangeEnd(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const;
	void SetPlaybackRangeStart(FFrameNumber NewStart);
	void SetPlaybackRangeEnd(FFrameNumber NewEnd);
	void SetSelectionRangeStart(FFrameNumber NewStart);
	void SetSelectionRangeEnd(FFrameNumber NewEnd);

	/**
	* Zoom the range by a given delta.
	* 
	* @param InDelta		The total amount to zoom by (+ve = zoom out, -ve = zoom in)
	* @param ZoomBias		Bias to apply to lower/upper extents of the range. (0 = lower, 0.5 = equal, 1 = upper)
	*/
	bool ZoomByDelta(float InDelta, float ZoomBias = 0.5f);
	/**
	* Pan the range by a given delta
	* 
	* @param InDelta		The total amount to pan by (+ve = pan forwards in time, -ve = pan backwards in time)
	*/
	void PanByDelta(float InDelta);

	FReply OnMouseButtonDown(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseButtonUp(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseMove(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	FReply OnMouseWheel(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
};
