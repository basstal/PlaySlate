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
	TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
	/** TRANS_EN:Scrub style provided on construction */
	ActActionSequence::ESequencerScrubberStyle ScrubStyle;
public:
	ActActionSequence::ESequencerScrubberStyle GetScrubStyle() const
	{
		return ScrubStyle;
	}

	TSharedPtr<INumericTypeInterface<double>> GetNumericTypeInterface() const
	{
		return NumericTypeInterface;
	}

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
};
