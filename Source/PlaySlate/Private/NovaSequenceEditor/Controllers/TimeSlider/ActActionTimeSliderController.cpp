#include "ActActionTimeSliderController.h"

#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"

#include "Utils/ActActionStaticUtil.h"

#include "Fonts/FontMeasure.h"


FActActionTimeSliderController::FActActionTimeSliderController(const ActActionSequence::FActActionTimeSliderArgs& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController)
	: Sequence(InSequenceController),
	  TimeSliderArgs(InArgs),
	  ScrubStyle(),
	  DistanceDragged(0),
	  MouseDragType(),
	  bMouseDownInRegion(false),
	  DragMarkIndex(0),
	  bPanning(false)
{
}

FActActionTimeSliderController::~FActActionTimeSliderController()
{
	Sequence.Reset();
}

bool FActActionTimeSliderController::GetGridMetrics(const float PhysicalWidth, const double InViewStart, const double InViewEnd, double& OutMajorInterval, int32& OutMinorDivisions) const
{
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);
	TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

	// ** TRANS_EN:Use the end of the view as the longest number
	FFrameRate FocusedDisplayRate = FFrameRate();
	FString TickString = TimeSliderArgs.NumericTypeInterface->ToString((InViewEnd * FocusedDisplayRate).FrameNumber.Value);
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

	float RoundedStartPixel = FMath::RoundToInt(FrameStartPixel);
	FrameEndPixel -= (FrameStartPixel - RoundedStartPixel);

	FrameStartPixel = RoundedStartPixel;
	FrameEndPixel = FMath::Max(FrameEndPixel, FrameStartPixel + 1);

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
	FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);

	// if (CheckSnapping && false/*Sequencer->GetSequencerSettings()->GetIsSnapEnabled()*/)
	// {
	// 	// FFrameNumber        SnappedFrameNumber = (MouseValue * GetDisplayRate()).FloorToFrame();
	// 	// FQualifiedFrameTime RoundedPlayFrame   = FQualifiedFrameTime(SnappedFrameNumber, GetDisplayRate());
	// 	// return RoundedPlayFrame.ConvertTo(GetTickResolution());
	// }
	// else
	// {
	return MouseValue * TimeSliderArgs.TickResolution.Get();
	// }
}

int32 FActActionTimeSliderController::OnPaintViewArea(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, bool bEnabled, const ActActionSequence::FActActionPaintViewAreaArgs& Args) const
{
	if (!Sequence.IsValid())
	{
		return LayerId;
	}

	const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
	ActActionSequence::FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, AllottedGeometry.Size);

	if (Args.PlaybackRangeArgs.IsSet())
	{
		ActActionSequence::FActActionPaintPlaybackRangeArgs PaintArgs = Args.PlaybackRangeArgs.GetValue();
		LayerId = DrawPlaybackRange(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, RangeToScreen, PaintArgs);
		LayerId = DrawSubSequenceRange(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, RangeToScreen, PaintArgs);
		PaintArgs.SolidFillOpacity = 0.f;
		LayerId = DrawSelectionRange(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, RangeToScreen, PaintArgs);
	}

	if (Args.bDisplayTickLines)
	{
		static FLinearColor TickColor(0.f, 0.f, 0.f, 0.3f);

		// Draw major tick lines in the section area
		ActActionSequence::FActActionDrawTickArgs DrawTickArgs;
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

		DrawTicks(OutDrawElements, LocalViewRange, RangeToScreen, DrawTickArgs);
	}

	if (Args.bDisplayMarkedFrames)
	{
		LayerId = DrawMarkedFrames(AllottedGeometry, RangeToScreen, OutDrawElements, LayerId, DrawEffects, false);
	}

	LayerId = DrawVerticalFrames(AllottedGeometry, RangeToScreen, OutDrawElements, LayerId, DrawEffects);

	if (Args.bDisplayScrubPosition)
	{
		FQualifiedFrameTime ScrubPosition = FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), TimeSliderArgs.TickResolution.Get());
		ActActionSequence::FActActionScrubberMetrics ScrubMetrics = GetScrubPixelMetrics(ScrubPosition, RangeToScreen);

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

	return LayerId;
}

int32 FActActionTimeSliderController::DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const
{
	if (!Sequence.IsValid())
	{
		return LayerId;
	}

	if (!TimeSliderArgs.PlaybackRange.IsSet())
	{
		return LayerId;
	}

	const uint8 OpacityBlend = TimeSliderArgs.SubSequenceRange.Get().IsSet() ? 128 : 255;

	TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();
	FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	const float PlaybackRangeL = RangeToScreen.InputToLocalX(PlaybackRange.GetLowerBoundValue() / TickResolution);
	const float PlaybackRangeR = RangeToScreen.InputToLocalX(PlaybackRange.GetUpperBoundValue() / TickResolution) - 1;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeL, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		Args.StartBrush,
		ESlateDrawEffect::None,
		FColor(32, 128, 32, OpacityBlend) // 120, 75, 50 (HSV)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeR - Args.BrushWidth, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		Args.EndBrush,
		ESlateDrawEffect::None,
		FColor(128, 32, 32, OpacityBlend) // 0, 75, 50 (HSV)
	);

	// Black tint for excluded regions
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(0.f, 0.f), FVector2D(PlaybackRangeL, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f * OpacityBlend / 255.f)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeR, 0.f), FVector2D(AllottedGeometry.Size.X - PlaybackRangeR, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f * OpacityBlend / 255.f)
	);

	return LayerId + 1;
}

int32 FActActionTimeSliderController::DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const
{
	if (!Sequence.IsValid())
	{
		return LayerId;
	}

	TOptional<TRange<FFrameNumber>> RangeValue;
	RangeValue = TimeSliderArgs.SubSequenceRange.Get(RangeValue);

	if (!RangeValue.IsSet() || RangeValue->IsEmpty())
	{
		return LayerId;
	}

	const FFrameRate Resolution = TimeSliderArgs.TickResolution.Get();
	const FFrameNumber LowerFrame = RangeValue.GetValue().GetLowerBoundValue();
	const FFrameNumber UpperFrame = RangeValue.GetValue().GetUpperBoundValue();

	const float SubSequenceRangeL = RangeToScreen.InputToLocalX(LowerFrame / Resolution) - 1;
	const float SubSequenceRangeR = RangeToScreen.InputToLocalX(UpperFrame / Resolution) + 1;

	static const FSlateBrush* LineBrushL(FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_L"));
	static const FSlateBrush* LineBrushR(FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_R"));

	FColor GreenTint(32, 128, 32); // 120, 75, 50 (HSV)
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeL, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		LineBrushL,
		ESlateDrawEffect::None,
		GreenTint
	);

	FColor RedTint(128, 32, 32); // 0, 75, 50 (HSV)
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR - Args.BrushWidth, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		LineBrushR,
		ESlateDrawEffect::None,
		RedTint
	);

	// Black tint for excluded regions
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(0.f, 0.f), FVector2D(SubSequenceRangeL, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR, 0.f), FVector2D(AllottedGeometry.Size.X - SubSequenceRangeR, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f)
	);

	// Hash applied to the left and right of the sequence bounds
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeL - 16.f, 0.f), FVector2D(16.f, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("Sequencer.Timeline.SubSequenceRangeHashL"),
		ESlateDrawEffect::None,
		GreenTint
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR, 0.f), FVector2D(16.f, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("Sequencer.Timeline.SubSequenceRangeHashR"),
		ESlateDrawEffect::None,
		RedTint
	);

	return LayerId + 1;
}

int32 FActActionTimeSliderController::DrawSelectionRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const
{
	if (!Sequence.IsValid())
	{
		return LayerId;
	}

	// ** TODO:可能不需要这个范围，而且这里应该考虑挪到Widget中
	// TRange<double> SelectionRange = TimeSliderArgs.SelectionRange.Get() / TimeSliderArgs.TickResolution.Get();
	//
	// if (!SelectionRange.IsEmpty())
	// {
	// 	const float SelectionRangeL = RangeToScreen.InputToLocalX(SelectionRange.GetLowerBoundValue());
	// 	const float SelectionRangeR = RangeToScreen.InputToLocalX(SelectionRange.GetUpperBoundValue()) - 1;
	// 	const auto DrawColor = FEditorStyle::GetSlateColor("SelectionColor").GetColor(FWidgetStyle());
	//
	// 	if (Args.SolidFillOpacity > 0.f)
	// 	{
	// 		FSlateDrawElement::MakeBox(
	// 			OutDrawElements,
	// 			LayerId + 1,
	// 			AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeL, 0.f), FVector2D(SelectionRangeR - SelectionRangeL, AllottedGeometry.Size.Y)),
	// 			FEditorStyle::GetBrush("WhiteBrush"),
	// 			ESlateDrawEffect::None,
	// 			DrawColor.CopyWithNewOpacity(Args.SolidFillOpacity)
	// 		);
	// 	}
	//
	// 	FSlateDrawElement::MakeBox(
	// 		OutDrawElements,
	// 		LayerId + 1,
	// 		AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeL, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
	// 		Args.StartBrush,
	// 		ESlateDrawEffect::None,
	// 		DrawColor
	// 	);
	//
	// 	FSlateDrawElement::MakeBox(
	// 		OutDrawElements,
	// 		LayerId + 1,
	// 		AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeR - Args.BrushWidth, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
	// 		Args.EndBrush,
	// 		ESlateDrawEffect::None,
	// 		DrawColor
	// 	);
	// }
	//
	return LayerId + 1;
}

void FActActionTimeSliderController::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, ActActionSequence::FActActionDrawTickArgs& InArgs) const
{
	if (!Sequence.IsValid())
	{
		return;
	}

	FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	FFrameRate DisplayRate = TimeSliderArgs.DisplayRate.Get();
	FPaintGeometry PaintGeometry = InArgs.AllottedGeometry.ToPaintGeometry();
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

	double MajorGridStep = 0.0;
	int32 MinorDivisions = 0;
	if (!GetGridMetrics(InArgs.AllottedGeometry.Size.X, ViewRange.GetLowerBoundValue(), ViewRange.GetUpperBoundValue(), MajorGridStep, MinorDivisions))
	{
		return;
	}

	if (InArgs.bOnlyDrawMajorTicks)
	{
		MinorDivisions = 0;
	}

	TArray<FVector2D> LinePoints;
	LinePoints.SetNumUninitialized(2);

	const bool bAntiAliasLines = false;

	const double FirstMajorLine = FMath::FloorToDouble(ViewRange.GetLowerBoundValue() / MajorGridStep) * MajorGridStep;
	const double LastMajorLine = FMath::CeilToDouble(ViewRange.GetUpperBoundValue() / MajorGridStep) * MajorGridStep;

	const float FlooredScrubPx = RangeToScreen.InputToLocalX(ConvertFrameTime(TimeSliderArgs.ScrubPosition.Get(), TickResolution, TimeSliderArgs.DisplayRate.Get()).FloorToFrame() / DisplayRate);

	for (double CurrentMajorLine = FirstMajorLine; CurrentMajorLine < LastMajorLine; CurrentMajorLine += MajorGridStep)
	{
		float MajorLinePx = RangeToScreen.InputToLocalX(CurrentMajorLine);

		LinePoints[0] = FVector2D(MajorLinePx, InArgs.TickOffset);
		LinePoints[1] = FVector2D(MajorLinePx, InArgs.TickOffset + InArgs.MajorTickHeight);

		// Draw each tick mark
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			InArgs.StartLayer,
			PaintGeometry,
			LinePoints,
			InArgs.DrawEffects,
			InArgs.TickColor,
			bAntiAliasLines
		);

		if (!InArgs.bOnlyDrawMajorTicks && !FMath::IsNearlyEqual(MajorLinePx, FlooredScrubPx, 3.f))
		{
			FString FrameString = TimeSliderArgs.NumericTypeInterface->ToString((CurrentMajorLine * TickResolution).RoundToFrame().Value);

			// Space the text between the tick mark but slightly above
			FVector2D TextOffset(MajorLinePx + 5.f, InArgs.bMirrorLabels ? 1.f : FMath::Abs(InArgs.AllottedGeometry.Size.Y - (InArgs.MajorTickHeight + 3.f)));
			FSlateDrawElement::MakeText(
				OutDrawElements,
				InArgs.StartLayer + 1,
				InArgs.AllottedGeometry.ToPaintGeometry(TextOffset, InArgs.AllottedGeometry.Size),
				FrameString,
				SmallLayoutFont,
				InArgs.DrawEffects,
				InArgs.TickColor * 0.65f
			);
		}

		for (int32 Step = 1; Step < MinorDivisions; ++Step)
		{
			// Compute the size of each tick mark.  If we are half way between to visible values display a slightly larger tick mark
			const float MinorTickHeight = ((MinorDivisions % 2 == 0) && (Step % (MinorDivisions / 2)) == 0) ? 6.0f : 2.0f;
			const float MinorLinePx = RangeToScreen.InputToLocalX(CurrentMajorLine + Step * MajorGridStep / MinorDivisions);

			LinePoints[0] = FVector2D(MinorLinePx, InArgs.bMirrorLabels ? 0.0f : FMath::Abs(InArgs.AllottedGeometry.Size.Y - MinorTickHeight));
			LinePoints[1] = FVector2D(MinorLinePx, LinePoints[0].Y + MinorTickHeight);

			// Draw each sub mark
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				InArgs.StartLayer,
				PaintGeometry,
				LinePoints,
				InArgs.DrawEffects,
				InArgs.TickColor,
				bAntiAliasLines
			);
		}
	}
}

int32 FActActionTimeSliderController::DrawMarkedFrames(const FGeometry& AllottedGeometry, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ESlateDrawEffect& DrawEffects, bool bDrawLabels) const
{
	return LayerId;
	// const TArray<FMovieSceneMarkedFrame>& MarkedFrames = TimeSliderArgs.MarkedFrames.Get();
	// const TArray<FMovieSceneMarkedFrame>& GlobalMarkedFrames = TimeSliderArgs.GlobalMarkedFrames.Get();
	// if (MarkedFrames.Num() < 1 && GlobalMarkedFrames.Num() < 1)
	// {
	// 	return LayerId;
	// }
	//
	// const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	// FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	//
	// FQualifiedFrameTime ScrubPosition = FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), GetTickResolution());
	// FScrubberMetrics ScrubMetrics = GetScrubPixelMetrics(ScrubPosition, RangeToScreen);
	// auto DrawFrameMarkers = ([=](const TArray<FMovieSceneMarkedFrame>& InMarkedFrames, FSlateWindowElementList& DrawElements, bool bFade)
	// {
	// 	for (const FMovieSceneMarkedFrame& MarkedFrame : InMarkedFrames)
	// 	{
	// 		double Seconds = MarkedFrame.FrameNumber / GetTickResolution();
	//
	// 		FLinearColor DrawColor = bFade ? MarkedFrame.Color.Desaturate(0.25f) : MarkedFrame.Color;
	// 		const float LinePos = RangeToScreen.InputToLocalX(Seconds);
	// 		TArray<FVector2D> LinePoints;
	// 		LinePoints.AddUninitialized(2);
	// 		LinePoints[0] = FVector2D(LinePos, 0.0f);
	// 		LinePoints[1] = FVector2D(LinePos, FMath::FloorToFloat(AllottedGeometry.Size.Y));
	//
	// 		FSlateDrawElement::MakeLines(
	// 			DrawElements,
	// 			LayerId + 1,
	// 			AllottedGeometry.ToPaintGeometry(),
	// 			LinePoints,
	// 			DrawEffects,
	// 			DrawColor,
	// 			false
	// 		);
	//
	// 		FString LabelString = MarkedFrame.Label;
	// 		if (bDrawLabels && !LabelString.IsEmpty())
	// 		{
	// 			// Draw the label next to the marked frame line
	// 			FVector2D TextSize = FontMeasureService->Measure(LabelString, SmallLayoutFont);
	//
	// 			// Flip the text position if getting near the end of the view range
	// 			static const float TextOffsetPx = 2.f;
	// 			bool bDrawLeft = (AllottedGeometry.Size.X - LinePos) < (TextSize.X + 14.f) - TextOffsetPx;
	// 			float TextPosition = bDrawLeft ? LinePos - TextSize.X - TextOffsetPx : LinePos + TextOffsetPx;
	//
	// 			FSlateDrawElement::MakeText(
	// 				DrawElements,
	// 				LayerId + 1,
	// 				AllottedGeometry.ToPaintGeometry(FVector2D(TextPosition, 0.f), TextSize),
	// 				LabelString,
	// 				SmallLayoutFont,
	// 				DrawEffects,
	// 				DrawColor
	// 			);
	// 		}
	// 	}
	// });
	//
	// DrawFrameMarkers(GlobalMarkedFrames, OutDrawElements, true);
	// DrawFrameMarkers(MarkedFrames, OutDrawElements, false);
	//
	// return LayerId + 1;
}

int32 FActActionTimeSliderController::DrawVerticalFrames(const FGeometry& AllottedGeometry, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ESlateDrawEffect& DrawEffects) const
{
	TSet<FFrameNumber> VerticalFrames = TimeSliderArgs.VerticalFrames.Get();
	if (VerticalFrames.Num() < 1)
	{
		return LayerId;
	}

	for (FFrameNumber TickFrame : VerticalFrames)
	{
		double Seconds = TickFrame / TimeSliderArgs.TickResolution.Get();

		const float LinePos = RangeToScreen.InputToLocalX(Seconds);
		TArray<FVector2D> LinePoints;
		LinePoints.AddUninitialized(2);
		LinePoints[0] = FVector2D(LinePos, 0.0f);
		LinePoints[1] = FVector2D(LinePos, FMath::FloorToFloat(AllottedGeometry.Size.Y));

		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			LinePoints,
			DrawEffects,
			FLinearColor(0.7f, 0.7f, 0.f, 0.4f),
			false
		);
	}

	return LayerId + 1;
}

FFrameTime FActActionTimeSliderController::ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen) const
{
	FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	double MouseSeconds = RangeToScreen.LocalXToInput(CursorPos.X);
	FFrameTime ScrubTime = MouseSeconds * TimeSliderArgs.TickResolution.Get();

	if (!Sequence.IsValid())
	{
		return ScrubTime;
	}

	// if (Sequencer->GetSequencerSettings()->GetIsSnapEnabled())
	// {
	// 	if (Sequencer->GetSequencerSettings()->GetSnapPlayTimeToInterval())
	// 	{
	// 		// Set the style of the scrub handle
	// 		if (Sequencer->GetScrubStyle() == ESequencerScrubberStyle::FrameBlock)
	// 		{
	// 			// Floor to the display frame
	// 			ScrubTime = ConvertFrameTime(ConvertFrameTime(ScrubTime, GetTickResolution(), GetDisplayRate()).FloorToFrame(), GetDisplayRate(), GetTickResolution());
	// 		}
	// 		else
	// 		{
	// 			// Snap (round) to display rate
	// 			ScrubTime = FFrameRate::Snap(ScrubTime, GetTickResolution(), GetDisplayRate());
	// 		}
	// 	}
	//
	// 	if (Sequencer->GetSequencerSettings()->GetSnapPlayTimeToKeys())
	// 	{
	// 		// SnapTimeToNearestKey will return ScrubTime unmodified if there is no key within range.
	// 		ScrubTime = SnapTimeToNearestKey(RangeToScreen, CursorPos.X, ScrubTime);
	// 	}
	// }

	// if (Sequencer->GetSequencerSettings()->ShouldKeepCursorInPlayRangeWhileScrubbing())
	// {
	// 	ScrubTime = UE::MovieScene::ClampToDiscreteRange(ScrubTime, TimeSliderArgs.PlaybackRange.Get());
	// }

	return ScrubTime;
}

void FActActionTimeSliderController::CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing)
{
	// The user can scrub past the viewing range of the time slider controller, so we clamp it to the view range.
	if (Sequence.IsValid())
	{
		ActActionSequence::FActActionAnimatedRange ViewRange = TimeSliderArgs.ViewRange.Get();

		// FFrameRate DisplayRate = Sequence->GetFocusedDisplayRate();
		FFrameRate TickResolution = Sequence.Pin()->GetFocusedTickResolution();

		FFrameTime LowerBound = (ViewRange.GetLowerBoundValue() * TickResolution).CeilToFrame();
		FFrameTime UpperBound = (ViewRange.GetUpperBoundValue() * TickResolution).FloorToFrame();

		// if (Sequencer->GetSequencerSettings()->GetIsSnapEnabled() && Sequencer->GetSequencerSettings()->GetSnapPlayTimeToInterval())
		// {
		// 	LowerBound = FFrameRate::Snap(LowerBound, TickResolution, DisplayRate);
		// 	UpperBound = FFrameRate::Snap(UpperBound, TickResolution, DisplayRate);
		// }

		NewValue = FMath::Clamp(NewValue, LowerBound, UpperBound);
	}

	// Manage the scrub position ourselves if its not bound to a delegate
	if (!TimeSliderArgs.ScrubPosition.IsBound())
	{
		TimeSliderArgs.ScrubPosition.Set(NewValue);
	}

	TimeSliderArgs.OnScrubPositionChanged.ExecuteIfBound(NewValue, bIsScrubbing);
}

void FActActionTimeSliderController::ClampViewRange(double& NewRangeMin, double& NewRangeMax)
{
	bool bNeedsClampSet = false;
	double NewClampRangeMin = TimeSliderArgs.ClampRange.Get().GetLowerBoundValue();
	if (NewRangeMin < TimeSliderArgs.ClampRange.Get().GetLowerBoundValue())
	{
		NewClampRangeMin = NewRangeMin;
		bNeedsClampSet = true;
	}

	double NewClampRangeMax = TimeSliderArgs.ClampRange.Get().GetUpperBoundValue();
	if (NewRangeMax > TimeSliderArgs.ClampRange.Get().GetUpperBoundValue())
	{
		NewClampRangeMax = NewRangeMax;
		bNeedsClampSet = true;
	}

	if (bNeedsClampSet)
	{
		SetClampRange(NewClampRangeMin, NewClampRangeMax);
	}
}

void FActActionTimeSliderController::SetClampRange(double NewRangeMin, double NewRangeMax)
{
	const TRange<double> NewRange(NewRangeMin, NewRangeMax);

	TimeSliderArgs.OnClampRangeChanged.ExecuteIfBound(NewRange);

	if (!TimeSliderArgs.ClampRange.IsBound())
	{
		// The  output is not bound to a delegate so we'll manage the value ourselves (no animation)
		TimeSliderArgs.ClampRange.Set(NewRange);
	}
}

void FActActionTimeSliderController::SetViewRange(double NewRangeMin, double NewRangeMax, ActActionSequence::EActActionViewRangeInterpolation Interpolation)
{
	// Clamp to a minimum size to avoid zero-sized or negative visible ranges
	double MinVisibleTimeRange = FFrameNumber(1) / TimeSliderArgs.TickResolution.Get();
	TRange<double> ExistingViewRange = TimeSliderArgs.ViewRange.Get();
	TRange<double> ExistingClampRange = TimeSliderArgs.ClampRange.Get();

	if (NewRangeMax == ExistingViewRange.GetUpperBoundValue())
	{
		if (NewRangeMin > NewRangeMax - MinVisibleTimeRange)
		{
			NewRangeMin = NewRangeMax - MinVisibleTimeRange;
		}
	}
	else if (NewRangeMax < NewRangeMin + MinVisibleTimeRange)
	{
		NewRangeMax = NewRangeMin + MinVisibleTimeRange;
	}

	// Clamp to the clamp range
	const TRange<double> NewRange = TRange<double>::Intersection(TRange<double>(NewRangeMin, NewRangeMax), ExistingClampRange);

	TimeSliderArgs.OnViewRangeChanged.ExecuteIfBound(NewRange, Interpolation);
	if (!TimeSliderArgs.ViewRange.IsBound())
	{
		// The  output is not bound to a delegate so we'll manage the value ourselves (no animation)
		TimeSliderArgs.ViewRange.Set(NewRange);
	}
}

bool FActActionTimeSliderController::HitTestRangeStart(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeStartPixel = RangeToScreen.InputToLocalX(Range.GetLowerBoundValue());

	// Hit test against the brush region to the right of the playback start position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeStartPixel - 4.0f && HitPixel <= RangeStartPixel + 10.0f;
}

bool FActActionTimeSliderController::HitTestRangeEnd(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeEndPixel = RangeToScreen.InputToLocalX(Range.GetUpperBoundValue());
	// Hit test against the brush region to the left of the playback end position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeEndPixel - 10.0f && HitPixel <= RangeEndPixel + 4.0f;
}

void FActActionTimeSliderController::SetPlaybackRangeStart(FFrameNumber NewStart)
{
	TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(PlaybackRange.GetUpperBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, PlaybackRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetPlaybackRangeEnd(FFrameNumber NewEnd)
{
	TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(PlaybackRange.GetLowerBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(PlaybackRange.GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive(NewEnd)));
	}
}

void FActActionTimeSliderController::SetSelectionRangeStart(FFrameNumber NewStart)
{
	TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, NewStart + 1));
	}
	else if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(SelectionRange.GetUpperBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, SelectionRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetSelectionRangeEnd(FFrameNumber NewEnd)
{
	TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewEnd - 1, NewEnd));
	}
	else if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(SelectionRange.GetLowerBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(SelectionRange.GetLowerBound(), NewEnd));
	}
}

bool FActActionTimeSliderController::ZoomByDelta(float InDelta, float ZoomBias)
{
	TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();
	double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const double OutputViewSize = LocalViewRangeMax - LocalViewRangeMin;
	const double OutputChange = OutputViewSize * InDelta;

	double NewViewOutputMin = LocalViewRangeMin - (OutputChange * ZoomBias);
	double NewViewOutputMax = LocalViewRangeMax + (OutputChange * (1.f - ZoomBias));

	if (NewViewOutputMin < NewViewOutputMax)
	{
		ClampViewRange(NewViewOutputMin, NewViewOutputMax);
		SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
		return true;
	}

	return false;
}

void FActActionTimeSliderController::PanByDelta(float InDelta)
{
	TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();

	double CurrentMin = LocalViewRange.GetLowerBoundValue();
	double CurrentMax = LocalViewRange.GetUpperBoundValue();

	// Adjust the delta to be a percentage of the current range
	InDelta *= 0.1f * (CurrentMax - CurrentMin);

	double NewViewOutputMin = CurrentMin + InDelta;
	double NewViewOutputMax = CurrentMax + InDelta;

	ClampViewRange(NewViewOutputMin, NewViewOutputMax);
	SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
}


FReply FActActionTimeSliderController::OnMouseButtonDown(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
	DistanceDragged = 0;
	MouseDownPosition[0] = MouseDownPosition[1] = MouseEvent.GetScreenSpacePosition();
	MouseDownGeometry = MyGeometry;
	bMouseDownInRegion = false;
	DragMarkIndex = INDEX_NONE;

	FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();
	FVector2D LocalPos = MouseDownGeometry.AbsoluteToLocal(CursorPos);
	if (LocalPos.Y >= 0 && LocalPos.Y < MouseDownGeometry.GetLocalSize().Y)
	{
		bMouseDownInRegion = true;
	}

	return FReply::Unhandled();
}

FReply FActActionTimeSliderController::OnMouseButtonUp(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bool bHandleLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && OwnerWidget.HasMouseCapture();
	bool bHandleRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && OwnerWidget.HasMouseCapture() && TimeSliderArgs.AllowZoom;

	ActActionSequence::FActActionScrubRangeToScreen RangeToScreen = ActActionSequence::FActActionScrubRangeToScreen(TimeSliderArgs.ViewRange.Get(), MyGeometry.Size);
	FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

	if (bHandleRightMouseButton)
	{
		if (!bPanning)
		{
			// // Open a context menu if allowed
			// if (ContextMenuSuppression == 0 && TimeSliderArgs.PlaybackRange.IsSet())
			// {
			// 	TSharedRef<SWidget> MenuContent = OpenSetPlaybackRangeMenu(MyGeometry, MouseEvent);
			// 	FSlateApplication::Get().PushMenu(
			// 		WidgetOwner.AsShared(),
			// 		MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath(),
			// 		MenuContent,
			// 		MouseEvent.GetScreenSpacePosition(),
			// 		FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
			// 	);
			//
			// 	return FReply::Handled().SetUserFocus(MenuContent, EFocusCause::SetDirectly).ReleaseMouseCapture();
			// }

			// return unhandled in case our parent wants to use our right mouse button to open a context menu
			if (DistanceDragged == 0.f)
			{
				return FReply::Unhandled().ReleaseMouseCapture();
			}
		}

		bPanning = false;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}
	else if (bHandleLeftMouseButton)
	{
		if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_START)
		{
			TimeSliderArgs.OnPlaybackRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_END)
		{
			TimeSliderArgs.OnPlaybackRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_START)
		{
			TimeSliderArgs.OnSelectionRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_END)
		{
			TimeSliderArgs.OnSelectionRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_MARK)
		{
			TimeSliderArgs.OnMarkEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SETTING_RANGE)
		{
			// Zooming
			FFrameTime MouseDownStart = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen);

			const bool bCanZoomIn = MouseTime > MouseDownStart;
			const bool bCanZoomOut = ViewRangeStack.Num() > 0;
			if (bCanZoomIn || bCanZoomOut)
			{
				TRange<double> ViewRange = TimeSliderArgs.ViewRange.Get();
				if (!bCanZoomIn)
				{
					ViewRange = ViewRangeStack.Pop();
				}

				if (bCanZoomIn)
				{
					// push the current value onto the stack
					ViewRangeStack.Add(ViewRange);

					ViewRange = TRange<double>(MouseDownStart.FrameNumber / TimeSliderArgs.TickResolution.Get(), MouseTime.FrameNumber / TimeSliderArgs.TickResolution.Get());
				}

				TimeSliderArgs.OnViewRangeChanged.ExecuteIfBound(ViewRange, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
				if (!TimeSliderArgs.ViewRange.IsBound())
				{
					// The output is not bound to a delegate so we'll manage the value ourselves
					TimeSliderArgs.ViewRange.Set(ViewRange);
				}
			}
		}
		else if (bMouseDownInRegion)
		{
			TimeSliderArgs.OnEndScrubberMovement.ExecuteIfBound();

			FFrameTime ScrubTime = MouseTime;
			FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();

			if (MouseDragType == ActActionSequence::EDragType::DRAG_SCRUBBING_TIME)
			{
				ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, CursorPos, RangeToScreen);
			}
			// else if (Sequence.IsValid() && Sequencer->GetSequencerSettings()->GetSnapPlayTimeToKeys())
			// {
			// 	ScrubTime = SnapTimeToNearestKey(RangeToScreen, CursorPos.X, ScrubTime);
			// }

			CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/false);
		}

		MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
		DistanceDragged = 0.f;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}

	bMouseDownInRegion = false;
	return FReply::Unhandled();
}

FReply FActActionTimeSliderController::OnMouseMove(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!Sequence.IsValid())
	{
		return FReply::Unhandled();
	}

	bool bHandleLeftMouseButton = MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bHandleRightMouseButton = MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && TimeSliderArgs.AllowZoom;

	if (bHandleRightMouseButton)
	{
		if (!bPanning)
		{
			DistanceDragged += FMath::Abs(MouseEvent.GetCursorDelta().X);
			if (DistanceDragged > FSlateApplication::Get().GetDragTriggerDistance())
			{
				bPanning = true;
			}
		}
		else
		{
			TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
			double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
			double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();

			ActActionSequence::FActActionScrubRangeToScreen ScaleInfo(LocalViewRange, MyGeometry.Size);
			FVector2D ScreenDelta = MouseEvent.GetCursorDelta();
			FVector2D InputDelta;
			InputDelta.X = ScreenDelta.X / ScaleInfo.PixelsPerInput;

			double NewViewOutputMin = LocalViewRangeMin - InputDelta.X;
			double NewViewOutputMax = LocalViewRangeMax - InputDelta.X;

			ClampViewRange(NewViewOutputMin, NewViewOutputMax);
			SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
		}
	}
	else if (bHandleLeftMouseButton)
	{
		TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
		ActActionSequence::FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, MyGeometry.Size);
		DistanceDragged += FMath::Abs(MouseEvent.GetCursorDelta().X);

		if (MouseDragType == ActActionSequence::EDragType::DRAG_NONE)
		{
			if (DistanceDragged > FSlateApplication::Get().GetDragTriggerDistance())
			{
				FFrameTime MouseDownFree = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen, false);

				const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
				const bool bLockedPlayRange = TimeSliderArgs.IsPlaybackRangeLocked.Get();
				const float MouseDownPixel = RangeToScreen.InputToLocalX(MouseDownFree / TickResolution);
				ActActionSequence::FActActionScrubberMetrics ScrubberMetrics = GetScrubPixelMetrics(FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), TickResolution), RangeToScreen, 4.0f);
				const bool bHitScrubber = ScrubberMetrics.HandleRangePx.Contains(MouseDownPixel);

				TRange<double> SelectionRange = TimeSliderArgs.SelectionRange.Get() / TickResolution;
				TRange<double> PlaybackRange = TimeSliderArgs.PlaybackRange.Get() / TickResolution;

				// Disable selection range test if it's empty so that the playback range scrubbing gets priority
				if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeEnd(RangeToScreen, SelectionRange, MouseDownPixel))
				{
					// selection range end scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_SELECTION_END;
					TimeSliderArgs.OnSelectionRangeBeginDrag.ExecuteIfBound();
				}
				else if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeStart(RangeToScreen, SelectionRange, MouseDownPixel))
				{
					// selection range start scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_SELECTION_START;
					TimeSliderArgs.OnSelectionRangeBeginDrag.ExecuteIfBound();
				}
				else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeEnd(RangeToScreen, PlaybackRange, MouseDownPixel))
				{
					// playback range end scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_PLAYBACK_END;
					TimeSliderArgs.OnPlaybackRangeBeginDrag.ExecuteIfBound();
				}
				else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeStart(RangeToScreen, PlaybackRange, MouseDownPixel))
				{
					// playback range start scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_PLAYBACK_START;
					TimeSliderArgs.OnPlaybackRangeBeginDrag.ExecuteIfBound();
				}
					// else if (!bHitScrubber && HitTestMark(RangeToScreen, MouseDownPixel, DragMarkIndex))
					// {
					// 	MouseDragType = ActActionSequence::EDragType::DRAG_MARK;
					// 	TimeSliderArgs.OnMarkBeginDrag.ExecuteIfBound();
					// }
				else if (FSlateApplication::Get().GetModifierKeys().AreModifersDown(EModifierKey::Control))
				{
					MouseDragType = ActActionSequence::EDragType::DRAG_SETTING_RANGE;
				}
				else if (bMouseDownInRegion)
				{
					MouseDragType = ActActionSequence::EDragType::DRAG_SCRUBBING_TIME;
					TimeSliderArgs.OnBeginScrubberMovement.ExecuteIfBound();
				}
			}
		}
		else
		{
			FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);
			FFrameTime ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

			// Set the start range time?
			if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_START)
			{
				SetPlaybackRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_END)
			{
				SetPlaybackRangeEnd(MouseTime.FrameNumber);
			}
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_START)
			{
				SetSelectionRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_END)
			{
				SetSelectionRangeEnd(MouseTime.FrameNumber);
			}
				// else if (MouseDragType == ActActionSequence::EDragType::DRAG_MARK)
				// {
				// 	SetMark(DragMarkIndex, MouseTime.FrameNumber);
				// }
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SCRUBBING_TIME)
			{
				// Delegate responsibility for clamping to the current viewrange to the client
				CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/true);
			}
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SETTING_RANGE)
			{
				MouseDownPosition[1] = MouseEvent.GetScreenSpacePosition();
			}
		}
	}

	if (DistanceDragged != 0.f && (bHandleLeftMouseButton || bHandleRightMouseButton))
	{
		return FReply::Handled().CaptureMouse(OwnerWidget.AsShared());
	}


	return FReply::Handled();
}

FReply FActActionTimeSliderController::OnMouseWheel(SWidget& OwnerWidget, const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TOptional<TRange<float>> NewTargetRange;

	if (TimeSliderArgs.AllowZoom && MouseEvent.IsControlDown())
	{
		float MouseFractionX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / MyGeometry.GetLocalSize().X;

		// If zooming on the current time, adjust mouse fractionX
		// if (Sequence.IsValid() && Sequencer->GetSequencerSettings()->GetZoomPosition() == ESequencerZoomPosition::SZP_CurrentTime)
		// {
		// 	const double ScrubPosition = TimeSliderArgs.ScrubPosition.Get() / GetTickResolution();
		// 	if (GetViewRange().Contains(ScrubPosition))
		// 	{
		// 		FScrubRangeToScreen RangeToScreen(GetViewRange(), MyGeometry.Size);
		// 		float TimePosition = RangeToScreen.InputToLocalX(ScrubPosition);
		// 		MouseFractionX = TimePosition / MyGeometry.GetLocalSize().X;
		// 	}
		// }

		const float ZoomDelta = -0.2f * MouseEvent.GetWheelDelta();
		if (ZoomByDelta(ZoomDelta, MouseFractionX))
		{
			return FReply::Handled();
		}
	}
	else if (MouseEvent.IsShiftDown())
	{
		PanByDelta(-MouseEvent.GetWheelDelta());
		return FReply::Handled();
	}

	return FReply::Unhandled();
}
