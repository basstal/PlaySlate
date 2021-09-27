#pragma once
#include "ActActionSequenceController.h"
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
	TSharedPtr<FActActionSequenceController> Sequence;
	/**
	 * 控制器相关参数
	 */
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs;
	/** TRANS_EN:Numeric type interface used for converting parsing and generating strings from numbers */
	// TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
	/** TRANS_EN:Scrub style provided on construction */
	ActActionSequence::ESequencerScrubberStyle ScrubStyle;
public:
	FActActionTimeSliderController(const ActActionSequence::FActActionTimeSliderArgs& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController);

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

	TSharedPtr<FActActionSequenceController> GetSequence() const
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
};
