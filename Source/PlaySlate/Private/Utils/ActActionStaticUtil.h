#pragma once
#include "ActActionSequenceUtil.h"
#include "Fonts/FontMeasure.h"

namespace ActActionSequence
{
	class ActActionStaticUtil
	{
	public:
		static float GetBoundsZOffset(const FBoxSphereBounds& Bounds)
		{
			// Return half the height of the bounds plus one to avoid ZFighting with the floor plane
			return Bounds.BoxExtent.Z + 1;
		}

		static FFrameNumber DiscreteExclusiveUpper(const TRangeBound<FFrameNumber>& InUpperBound)
		{
			check(!InUpperBound.IsOpen());

			// Add one for inclusive upper bounds since they finish on the next subsequent frame
			static const int32 Offsets[] = {0, 1};
			const int32 OffsetIndex = (int32)InUpperBound.IsInclusive();

			return InUpperBound.GetValue() + Offsets[OffsetIndex];
		}

		static FFrameNumber DiscreteInclusiveLower(const TRangeBound<FFrameNumber>& InLowerBound)
		{
			check(!InLowerBound.IsOpen());

			// Add one for exclusive lower bounds since they start on the next subsequent frame
			static const int32 Offsets[] = {0, 1};
			const int32 OffsetIndex = (int32)InLowerBound.IsExclusive();

			return InLowerBound.GetValue() + Offsets[OffsetIndex];
		}

		/**
		 * Get the pixel metrics of the Scrubber
		 * @param ScrubTime			The qualified time of the scrubber
		 * @param RangeToScreen		Range to screen helper
		 * @param DilationPixels		Number of pixels to dilate the handle by
		 * @return FActActionScrubberMetrics
		 */
		static FActActionScrubberMetrics GetScrubPixelMetrics(const FFrameRate& DisplayRate, const FQualifiedFrameTime& ScrubTime, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, float DilationPixels = 0.0f, const ActActionSequence::ESequencerScrubberStyle& ScrubberStyle = ActActionSequence::ESequencerScrubberStyle())
		{
			ActActionSequence::FActActionScrubberMetrics Metrics;

			const FFrameNumber Frame = ScrubTime.ConvertTo(DisplayRate).FloorToFrame();

			float FrameStartPixel = RangeToScreen.InputToLocalX(Frame / DisplayRate);
			float FrameEndPixel = RangeToScreen.InputToLocalX((Frame + 1) / DisplayRate) - 1;

			float RoundedStartPixel = FMath::RoundToInt(FrameStartPixel);
			FrameEndPixel -= (FrameStartPixel - RoundedStartPixel);

			FrameStartPixel = RoundedStartPixel;
			FrameEndPixel = FMath::Max(FrameEndPixel, FrameStartPixel + 1);

			// Store off the pixel width of the frame
			Metrics.FrameExtentsPx = TRange<float>(FrameStartPixel, FrameEndPixel);

			// Set the style of the scrub handle
			Metrics.Style = ScrubberStyle;

			// Always draw the extents on the section area for frame block styles
			Metrics.bDrawExtents = Metrics.Style == ActActionSequence::ESequencerScrubberStyle::FrameBlock;

			static float MinScrubSize = 14.f;
			// If it's vanilla style or too small to show the frame width, set that up
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

		static bool GetGridMetrics(TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface, const float PhysicalWidth, const double InViewStart, const double InViewEnd, double& OutMajorInterval, int32& OutMinorDivisions)
		{
			FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
			TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

			// ** Use the end of the view as the longest number
			FFrameRate FocusedDisplayRate = FFrameRate();
			FString TickString = NumericTypeInterface->ToString((InViewEnd * FocusedDisplayRate).FrameNumber.Value);
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
	};
}
