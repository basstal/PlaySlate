#include "ActActionTimeSliderWidget.h"

#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineSlider.h"

#include "Fonts/FontMeasure.h"
#include "Common/NovaStaticFunction.h"

void SActActionTimeSliderWidget::Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineSlider>& InTimeSliderController)
{
	TimeSliderController = InTimeSliderController;
}

int32 SActActionTimeSliderWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs = TimeSliderController.Pin()->GetTimeSliderArgs();
	const TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
	const float LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const float LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	const float LocalSequenceLength = LocalViewRangeMax - LocalViewRangeMin;
	if (LocalSequenceLength <= 0)
	{
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}

	ActActionSequence::FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, AllottedGeometry.Size);

	// ** draw tick marks
	constexpr float MajorTickHeight = 9.0f;
	const ESlateDrawEffect DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const bool bMirrorLabels = TimeSliderController.Pin()->bMirrorLabels;
	ActActionSequence::FActActionDrawTickArgs DrawTickArgs;
	DrawTickArgs.AllottedGeometry = AllottedGeometry;
	DrawTickArgs.bMirrorLabels = bMirrorLabels;
	DrawTickArgs.bOnlyDrawMajorTicks = false;
	DrawTickArgs.TickColor = FLinearColor::White;
	DrawTickArgs.CullingRect = MyCullingRect;
	DrawTickArgs.DrawEffects = DrawEffects;
	// Draw major ticks under sections
	DrawTickArgs.StartLayer = LayerId;
	// Draw the tick the entire height of the section area
	DrawTickArgs.TickOffset = bMirrorLabels ? 0.0f : FMath::Abs(AllottedGeometry.Size.Y - MajorTickHeight);
	DrawTickArgs.MajorTickHeight = MajorTickHeight;
	TimeSliderController.Pin()->DrawTicks(OutDrawElements, LocalViewRange, RangeToScreen, DrawTickArgs);

	// Draw the scrub handle
	FQualifiedFrameTime ScrubPosition = FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), TimeSliderArgs.TickResolution.Get());
	const FFrameRate DisplayRate = TimeSliderArgs.DisplayRate.Get();
	ActActionSequence::FActActionScrubberMetrics ScrubberMetrics = NovaStaticFunction::GetScrubPixelMetrics(DisplayRate, ScrubPosition, RangeToScreen);
	const float HandleStart = ScrubberMetrics.HandleRangePx.GetLowerBoundValue();
	const float HandleEnd = ScrubberMetrics.HandleRangePx.GetUpperBoundValue();

	FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(FVector2D(HandleStart, 0), FVector2D(HandleEnd - HandleStart, AllottedGeometry.Size.Y));
	FLinearColor ScrubColor = InWidgetStyle.GetColorAndOpacityTint();
	// TODO:Sequencer this color should be specified in the style
	ScrubColor.A *= 0.75f;
	ScrubColor.B *= 0.1f;
	ScrubColor.G *= 0.2f;

	const FSlateBrush* FrameBlockScrubHandleUpBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.FrameBlockScrubHandleUp"));
	const FSlateBrush* FrameBlockScrubHandleDownBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.FrameBlockScrubHandleDown"));
	const FSlateBrush* VanillaScrubHandleUpBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.VanillaScrubHandleUp"));
	const FSlateBrush* VanillaScrubHandleDownBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.VanillaScrubHandleDown"));
	const FSlateBrush* Brush = ScrubberMetrics.Style == ENovaSequencerScrubberStyle::Vanilla ? (bMirrorLabels ? VanillaScrubHandleUpBrush : VanillaScrubHandleDownBrush) : (bMirrorLabels ? FrameBlockScrubHandleUpBrush : FrameBlockScrubHandleDownBrush);
	const int32 ArrowLayer = LayerId + 2;
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		ArrowLayer,
		PaintGeometry,
		Brush,
		DrawEffects,
		ScrubColor
	);

	// Draw the current time next to the scrub handle
	FLinearColor TextColor = FLinearColor::Yellow;
	FString FrameString = TimeSliderArgs.ScrubPositionText.Get();
	if (!TimeSliderArgs.ScrubPositionText.IsSet())
	{
		FrameString = TimeSliderArgs.NumericTypeInterface->ToString(TimeSliderArgs.ScrubPosition.Get().GetFrame().Value);
	}
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const TSharedRef<FSlateFontMeasure> SlateFontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize = SlateFontMeasureService->Measure(FrameString, SmallLayoutFont);

	// Flip the text position if getting near the end of the view range
	static constexpr float TextOffsetPx = 2.f;
	bool bDrawLeft = (AllottedGeometry.Size.X - HandleEnd) < ((TextSize.X + 14.f) - TextOffsetPx);
	float TextPosition = bDrawLeft ? (HandleStart - TextSize.X - TextOffsetPx) : (HandleEnd + TextOffsetPx);
	FVector2D TextOffset(TextPosition, bMirrorLabels ? (DrawTickArgs.AllottedGeometry.Size.Y - TextSize.Y) : 0.0f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		DrawTickArgs.StartLayer + 1,
		DrawTickArgs.AllottedGeometry.ToPaintGeometry(TextOffset, TextSize),
		FrameString,
		SmallLayoutFont,
		DrawTickArgs.DrawEffects,
		TextColor
	);

	if (TimeSliderController.IsValid() && TimeSliderController.Pin()->MouseDragType == ENovaDragType::DRAG_SETTING_RANGE)
	{
		TSharedRef<FActEventTimelineSlider> TimeSliderControllerRef = TimeSliderController.Pin().ToSharedRef();
		FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
		ActActionSequence::FActActionAnimatedRange AnimatedRange = TimeSliderArgs.ViewRange.Get();
		FFrameTime MouseDownTime[2];
		ActActionSequence::FActActionScrubRangeToScreen MouseDownRange(AnimatedRange, TimeSliderControllerRef->MouseDownGeometry.Size);
		MouseDownTime[0] = TimeSliderControllerRef->ComputeFrameTimeFromMouse(TimeSliderControllerRef->MouseDownGeometry, TimeSliderControllerRef->MouseDownPosition[0], MouseDownRange);
		MouseDownTime[1] = TimeSliderControllerRef->ComputeFrameTimeFromMouse(TimeSliderControllerRef->MouseDownGeometry, TimeSliderControllerRef->MouseDownPosition[1], MouseDownRange);
		float MouseStartPosX = RangeToScreen.InputToLocalX(MouseDownTime[0] / TickResolution);
		float MouseEndPosX = RangeToScreen.InputToLocalX(MouseDownTime[1] / TickResolution);
		float RangePosX = MouseStartPosX < MouseEndPosX ? MouseStartPosX : MouseEndPosX;
		float RangeSizeX = FMath::Abs(MouseStartPosX - MouseEndPosX);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2D(RangePosX, 0.0f), FVector2D(RangeSizeX, AllottedGeometry.Size.Y)),
			bMirrorLabels ? VanillaScrubHandleDownBrush : VanillaScrubHandleUpBrush,
			DrawEffects,
			MouseStartPosX < MouseEndPosX ? FLinearColor(0.5f, 0.5f, 0.5f) : FLinearColor(0.25f, 0.3f, 0.3f)
		);
	}

	return ArrowLayer;
}


FReply SActActionTimeSliderWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TimeSliderController.Pin()->OnMouseButtonDown(MyGeometry, MouseEvent);
	return FReply::Handled().CaptureMouse(AsShared()).PreventThrottling();
}

FReply SActActionTimeSliderWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return TimeSliderController.Pin()->OnMouseButtonUp(MyGeometry, MouseEvent);
}

FReply SActActionTimeSliderWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return TimeSliderController.Pin()->OnMouseMove(MyGeometry, MouseEvent);
}

FReply SActActionTimeSliderWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return TimeSliderController.Pin()->OnMouseWheel(MyGeometry, MouseEvent);
}
