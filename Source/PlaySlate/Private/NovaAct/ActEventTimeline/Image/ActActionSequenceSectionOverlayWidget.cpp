#include "ActActionSequenceSectionOverlayWidget.h"

#include "Common/NovaDataBinding.h"
#include "Common/NovaStaticFunction.h"

void SActActionSequenceSectionOverlayWidget::Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineImage>& InActActionSequenceSectionOverlayController)
{
	bDisplayScrubPosition = InArgs._DisplayScrubPosition;
	bDisplayTickLines = InArgs._DisplayTickLines;
	bDisplayMarkedFrames = InArgs._DisplayMarkedFrames;
	ActActionSequenceSectionOverlayController = InActActionSequenceSectionOverlayController;
}

int32 SActActionSequenceSectionOverlayWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	FActActionPaintViewAreaArgs PaintArgs;
	PaintArgs.bDisplayTickLines = bDisplayTickLines.Get();
	PaintArgs.bDisplayScrubPosition = bDisplayScrubPosition.Get();
	PaintArgs.bDisplayMarkedFrames = bDisplayMarkedFrames.Get();
	const ESlateDrawEffect DrawEffects = ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	TRange<float> LocalViewRange = *ActEventTimelineArgs->ViewRange;
	const FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, AllottedGeometry.Size);
	if (PaintArgs.bDisplayTickLines)
	{
		static FLinearColor TickColor(0.0f, 0.0f, 0.0f, 0.3f);
		// Draw major tick lines in the section area
		FActActionDrawTickArgs DrawTickArgs;
		DrawTickArgs.AllottedGeometry = AllottedGeometry;
		DrawTickArgs.bMirrorLabels = false;
		DrawTickArgs.bOnlyDrawMajorTicks = true;
		DrawTickArgs.TickColor = TickColor;
		DrawTickArgs.CullingRect = MyCullingRect;
		DrawTickArgs.DrawEffects = DrawEffects;
		// Draw major ticks under sections
		DrawTickArgs.StartLayer = LayerId - 1;
		// Draw the tick the entire height of the section area
		DrawTickArgs.TickOffset = 0.0f;
		DrawTickArgs.MajorTickHeight = AllottedGeometry.Size.Y;
		ActActionSequenceSectionOverlayController.Pin()->DrawTicks(OutDrawElements, LocalViewRange, RangeToScreen, DrawTickArgs);
	}

	if (PaintArgs.bDisplayScrubPosition)
	{
		FQualifiedFrameTime ScrubPosition = FQualifiedFrameTime(*ActEventTimelineArgs->CurrentTime, ActEventTimelineArgs->TickResolution);
		const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
		FActActionScrubberMetrics ScrubMetrics = NovaStaticFunction::GetScrubPixelMetrics(TickResolution, ScrubPosition, RangeToScreen);
		if (ScrubMetrics.bDrawExtents)
		{
			// Draw a box for the scrub position
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId + 1,
				AllottedGeometry.ToPaintGeometry(FVector2D(ScrubMetrics.FrameExtentsPx.GetLowerBoundValue(), 0.0f), FVector2D(ScrubMetrics.FrameExtentsPx.Size<float>(), AllottedGeometry.Size.Y)),
				FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.ScrubFill")),
				DrawEffects,
				FLinearColor::White.CopyWithNewOpacity(0.5f)
			);
		}

		// Draw a line for the scrub position
		TArray<FVector2D> LinePoints;
		float LinePos = RangeToScreen.InputToLocalX(ScrubPosition.AsSeconds());
		LinePoints.AddUninitialized(2);
		LinePoints[0] = FVector2D(LinePos, 0.0f);
		LinePoints[1] = FVector2D(LinePos, FMath::FloorToFloat(AllottedGeometry.Size.Y));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			LinePoints,
			DrawEffects,
			FLinearColor(1.f, 1.f, 1.f, .5f),
			false
		);
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}
