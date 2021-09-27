#include "ActActionTimeSliderController.h"

#include "Fonts/FontMeasure.h"

FActActionTimeSliderController::FActActionTimeSliderController(const ActActionSequence::FActActionTimeSliderArgs& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController)
	: Sequence(InSequenceController),
	  TimeSliderArgs(InArgs),
	  ScrubStyle()
{
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
	FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);

	// ** TODO:Sequence Settings
	// if (CheckSnapping && false/*Sequencer->GetSequencerSettings()->GetIsSnapEnabled()*/)
	// {
	// 	// FFrameNumber        SnappedFrameNumber = (MouseValue * GetDisplayRate()).FloorToFrame();
	// 	// FQualifiedFrameTime RoundedPlayFrame   = FQualifiedFrameTime(SnappedFrameNumber, GetDisplayRate());
	// 	// return RoundedPlayFrame.ConvertTo(GetTickResolution());
	// }
	// else
	// {
	return MouseValue * GetTimeSliderArgs().TickResolution.Get();
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
		{
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
		}

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
		{
			float LinePos = RangeToScreen.InputToLocalX(ScrubPosition.AsSeconds());

			LinePoints.AddUninitialized(2);
			LinePoints[0] = FVector2D(LinePos, 0.0f);
			LinePoints[1] = FVector2D(LinePos, FMath::FloorToFloat(AllottedGeometry.Size.Y));
		}

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

	TRange<double> SelectionRange = TimeSliderArgs.SelectionRange.Get() / TimeSliderArgs.TickResolution.Get();

	if (!SelectionRange.IsEmpty())
	{
		const float SelectionRangeL = RangeToScreen.InputToLocalX(SelectionRange.GetLowerBoundValue());
		const float SelectionRangeR = RangeToScreen.InputToLocalX(SelectionRange.GetUpperBoundValue()) - 1;
		const auto DrawColor = FEditorStyle::GetSlateColor("SelectionColor").GetColor(FWidgetStyle());

		if (Args.SolidFillOpacity > 0.f)
		{
			FSlateDrawElement::MakeBox(
				OutDrawElements,
				LayerId + 1,
				AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeL, 0.f), FVector2D(SelectionRangeR - SelectionRangeL, AllottedGeometry.Size.Y)),
				FEditorStyle::GetBrush("WhiteBrush"),
				ESlateDrawEffect::None,
				DrawColor.CopyWithNewOpacity(Args.SolidFillOpacity)
			);
		}

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeL, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
			Args.StartBrush,
			ESlateDrawEffect::None,
			DrawColor
		);

		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2D(SelectionRangeR - Args.BrushWidth, 0.f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
			Args.EndBrush,
			ESlateDrawEffect::None,
			DrawColor
		);
	}

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
