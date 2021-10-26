﻿#include "ActEventTimelineImage.h"

#include "ActEventTimelineSlider.h"
#include "PlaySlate.h"

#include "NovaAct/ActEventTimeline/Image/ActActionSequenceSectionOverlayWidget.h"

FActEventTimelineImage::FActEventTimelineImage(const TSharedRef<FActEventTimelineSlider>& InActActionTimeSliderController)
	: ActActionTimeSliderController(InActActionTimeSliderController)
{
}

FActEventTimelineImage::~FActEventTimelineImage()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActEventTimelineImage::~FActEventTimelineImage"));
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

void FActEventTimelineImage::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActActionScrubRangeToScreen& RangeToScreen, const FActActionDrawTickArgs& InArgs) const
{
	if (ActActionTimeSliderController.IsValid())
	{
		ActActionTimeSliderController.Pin()->DrawTicks(OutDrawElements, ViewRange, RangeToScreen, InArgs);
	}
}

// FActEventTimelineArgs& FActEventTimelineImage::GetTimeSliderArgs() const
// {
// 	return ActEventTimelineSlider.Pin()->GetTimeSliderArgs();
// }