#include "ActActionTimeSliderController.h"

#include "Fonts/FontMeasure.h"


bool FActActionTimeSliderController::GetGridMetrics(const float PhysicalWidth, const double InViewStart, const double InViewEnd, double& OutMajorInterval, int32& OutMinorDivisions) const
{
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
	TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	// ** TRANS_EN:Use the end of the view as the longest number
	FFrameRate FocusedDisplayRate = FFrameRate();
	FString TickString = GetNumericTypeInterface()->ToString((InViewEnd * FocusedDisplayRate).FrameNumber.Value);
	FVector2D MaxTextSize = FontMeasureService->Measure(TickString, SmallLayoutFont);

	float MinTickPx = MaxTextSize.X + 5.f;
	float DesiredMajorTickPx = MaxTextSize.X * 2.0f;

	if (PhysicalWidth > 0)
	{
		return FocusedDisplayRate.ComputeGridSpacing(
			PhysicalWidth / (InViewEnd - InViewStart),
			OutMajorInterval,
			OutMinorDivisions,
			MinTickPx,
			DesiredMajorTickPx);
	}

	return false;
}

ActActionSequence::FActActionScrubberMetrics FActActionTimeSliderController::GetScrubPixelMetrics(const FQualifiedFrameTime& ScrubTime, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, float DilationPixels) const
{
	FFrameRate DisplayRate = GetTimeSliderArgs().DisplayRate.Get();

	ActActionSequence::FActActionScrubberMetrics Metrics;

	const FFrameNumber Frame = ScrubTime.ConvertTo(DisplayRate).FloorToFrame();

	float FrameStartPixel = RangeToScreen.InputToLocalX(Frame / DisplayRate);
	float FrameEndPixel = RangeToScreen.InputToLocalX((Frame + 1) / DisplayRate) - 1;

	{
		float RoundedStartPixel = FMath::RoundToInt(FrameStartPixel);
		FrameEndPixel -= (FrameStartPixel - RoundedStartPixel);

		FrameStartPixel = RoundedStartPixel;
		FrameEndPixel = FMath::Max(FrameEndPixel, FrameStartPixel + 1);
	}

	// TRANS_EN:Store off the pixel width of the frame
	Metrics.FrameExtentsPx = TRange<float>(FrameStartPixel, FrameEndPixel);

	// TRANS_EN:Set the style of the scrub handle
	Metrics.Style = GetScrubStyle();

	// TRANS_EN:Always draw the extents on the section area for frame block styles
	Metrics.bDrawExtents = Metrics.Style == ActActionSequence::ESequencerScrubberStyle::FrameBlock;

	static float MinScrubSize = 14.f;
	// TRANS_EN:If it's vanilla style or too small to show the frame width, set that up
	if (Metrics.Style == ActActionSequence::ESequencerScrubberStyle::Vanilla || FrameEndPixel - FrameStartPixel < MinScrubSize)
	{
		Metrics.Style = ActActionSequence::ESequencerScrubberStyle::Vanilla;

		float ScrubPixel = RangeToScreen.InputToLocalX(ScrubTime.AsSeconds());
		Metrics.HandleRangePx = TRange<float>(ScrubPixel - MinScrubSize * .5f, ScrubPixel + MinScrubSize * .5f);
	}
	else
	{
		Metrics.HandleRangePx = Metrics.FrameExtentsPx;
	}

	return Metrics;
}

FFrameTime FActActionTimeSliderController::ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping) const
{
	FVector2D CursorPos  = Geometry.AbsoluteToLocal( ScreenSpacePosition );
	double    MouseValue = RangeToScreen.LocalXToInput( CursorPos.X );

	// ** TODO:Sequence Settings
	if (CheckSnapping && false/*Sequencer->GetSequencerSettings()->GetIsSnapEnabled()*/)
	{
		// FFrameNumber        SnappedFrameNumber = (MouseValue * GetDisplayRate()).FloorToFrame();
		// FQualifiedFrameTime RoundedPlayFrame   = FQualifiedFrameTime(SnappedFrameNumber, GetDisplayRate());
		// return RoundedPlayFrame.ConvertTo(GetTickResolution());
	}
	else
	{
		return MouseValue * GetTimeSliderArgs().TickResolution.Get();
	}
}
