#include "ActActionSequenceSectionOverlay.h"

void SActActionSequenceSectionOverlay::Construct(const FArguments& InArgs, const TSharedRef<FActActionTimeSliderController>& InTimeSliderController)
{
	bDisplayScrubPosition = InArgs._DisplayScrubPosition;
	bDisplayTickLines = InArgs._DisplayTickLines;
	bDisplayMarkedFrames = InArgs._DisplayMarkedFrames;
	PaintPlaybackRangeArgs = InArgs._PaintPlaybackRangeArgs;
	TimeSliderController = InTimeSliderController;
}

int32 SActActionSequenceSectionOverlay::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	ActActionSequence::FActActionPaintViewAreaArgs PaintArgs;
	PaintArgs.bDisplayTickLines = bDisplayTickLines.Get();
	PaintArgs.bDisplayScrubPosition = bDisplayScrubPosition.Get();
	PaintArgs.bDisplayMarkedFrames = bDisplayMarkedFrames.Get();

	if (PaintPlaybackRangeArgs.IsSet())
	{
		PaintArgs.PlaybackRangeArgs = PaintPlaybackRangeArgs.Get();
	}

	TimeSliderController->OnPaintViewArea(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, ShouldBeEnabled(bParentEnabled), PaintArgs);

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
