#include "ActImageThickLine.h"

#include "PlaySlate.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/ActEventTimeline/Slider/ActSliderWidget.h"


void SActImageThickLine::Construct(const FArguments& InArgs)
{
	Clipping = EWidgetClipping::ClipToBounds;
	Visibility = EVisibility::HitTestInvisible;
}

int32 SActImageThickLine::OnPaint(const FPaintArgs& Args,
                                  const FGeometry& AllottedGeometry,
                                  const FSlateRect& MyCullingRect,
                                  FSlateWindowElementList& OutDrawElements,
                                  int32 LayerId,
                                  const FWidgetStyle& InWidgetStyle,
                                  bool bParentEnabled) const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	auto LocalViewRange = *ActEventTimelineArgs->ViewRange;
	const FActSliderScrubRangeToScreen RangeToScreen(LocalViewRange, AllottedGeometry.Size);
	// Draw major tick lines in the section area
	FActDrawTickArgs DrawTickArgs;
	{
		DrawTickArgs.AllottedGeometry = AllottedGeometry;
		DrawTickArgs.bMirrorLabels = false;
		DrawTickArgs.bOnlyDrawMajorTicks = true;
		DrawTickArgs.TickColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.2f);
		DrawTickArgs.CullingRect = MyCullingRect;
		DrawTickArgs.DrawEffects = ShouldBeEnabled(bParentEnabled) ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
		// Draw major ticks under sections
		DrawTickArgs.StartLayer = LayerId - 1;
		// Draw the tick the entire height of the section area
		DrawTickArgs.TickOffset = 0.0f;
		DrawTickArgs.MajorTickHeight = AllottedGeometry.Size.Y;
	}
	SActSliderWidget::DrawTicks(OutDrawElements, LocalViewRange, RangeToScreen, DrawTickArgs);

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
}


SActImageThickLine::~SActImageThickLine()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActImageThickLine::~SActImageThickLine"));
}
