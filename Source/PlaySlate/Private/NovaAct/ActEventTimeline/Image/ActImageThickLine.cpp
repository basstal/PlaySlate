#include "ActImageThickLine.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Slider/ActSliderWidget.h"


void SActImageThickLine::Construct(const FArguments& InArgs)
{
	bDisplayMarkedFrames = InArgs._DisplayMarkedFrames;
	PaintPlaybackRangeArgs = InArgs._PaintPlaybackRangeArgs;
	// ActActionSequenceSectionOverlayWidget = SNew(SActImageScrubPosition, SharedThis(this))
	// 		.Visibility(EVisibility::HitTestInvisible)
	// 		// .DisplayScrubPosition(InOverlayWidgetType == ENovaSectionOverlayWidgetType::ScrubPosition)
	// 		// .DisplayTickLines(InOverlayWidgetType == ENovaSectionOverlayWidgetType::TickLines)
	// 		.DisplayMarkedFrames(InOverlayWidgetType == ENovaSectionOverlayWidgetType::ScrubPosition)
	// 		.Clipping(EWidgetClipping::ClipToBounds);
}

int32 SActImageThickLine::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	// FActActionPaintViewAreaArgs PaintArgs;
	// // PaintArgs.bDisplayTickLines = bDisplayTickLines.Get();
	// // PaintArgs.bDisplayScrubPosition = bDisplayScrubPosition.Get();
	// PaintArgs.bDisplayMarkedFrames = bDisplayMarkedFrames.Get();
	const ESlateDrawEffect DrawEffects = ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	TRange<float> LocalViewRange = *ActEventTimelineArgs->ViewRange;
	const FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, AllottedGeometry.Size);
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
	SActSliderWidget::DrawTicks(OutDrawElements, LocalViewRange, RangeToScreen, DrawTickArgs);


	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}


SActImageThickLine::~SActImageThickLine()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActImageThickLine::~SActImageThickLine"));
}

// void SActImageThickLine::MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType InOverlayWidgetType)
// {
// 	
// }

// void SActImageThickLine::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActActionScrubRangeToScreen& RangeToScreen, const FActActionDrawTickArgs& InArgs) const
// {
// 	
// }

// FActEventTimelineArgs& SActImageThickLine::GetTimeSliderArgs() const
// {
// 	return ActEventTimelineSlider.Pin()->GetTimeSliderArgs();
// }
