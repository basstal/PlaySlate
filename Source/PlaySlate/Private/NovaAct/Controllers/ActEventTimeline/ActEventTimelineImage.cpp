#include "ActEventTimelineImage.h"

#include "ActEventTimelineSlider.h"
#include "PlaySlate.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineBrain.h"
#include "NovaAct/Widgets/ActEventTimeline/Image/ActActionSequenceSectionOverlayWidget.h"

FActEventTimelineImage::FActEventTimelineImage(const TSharedRef<FActEventTimelineSlider>& InActActionTimeSliderController)
	: ActActionTimeSliderController(InActActionTimeSliderController)
{
}

FActEventTimelineImage::~FActEventTimelineImage()
{
	UE_LOG(LogActAction, Log, TEXT("FActEventTimelineImage::~FActEventTimelineImage"));
}

void FActEventTimelineImage::MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType InOverlayWidgetType)
{
	ActActionSequenceSectionOverlayWidget = SNew(SActActionSequenceSectionOverlayWidget, SharedThis(this))
			.Visibility(EVisibility::HitTestInvisible)
			.DisplayScrubPosition(InOverlayWidgetType == ENovaSectionOverlayWidgetType::ScrubPosition)
			.DisplayTickLines(InOverlayWidgetType == ENovaSectionOverlayWidgetType::TickLines)
			.DisplayMarkedFrames(InOverlayWidgetType == ENovaSectionOverlayWidgetType::ScrubPosition)
			.Clipping(EWidgetClipping::ClipToBounds);
}

void FActEventTimelineImage::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionDrawTickArgs& InArgs) const
{
	if (ActActionTimeSliderController.IsValid())
	{
		ActActionTimeSliderController.Pin()->DrawTicks(OutDrawElements, ViewRange, RangeToScreen, InArgs);
	}
}

ActActionSequence::FActActionTimeSliderArgs& FActEventTimelineImage::GetTimeSliderArgs() const
{
	return ActActionTimeSliderController.Pin()->GetTimeSliderArgs();
}
