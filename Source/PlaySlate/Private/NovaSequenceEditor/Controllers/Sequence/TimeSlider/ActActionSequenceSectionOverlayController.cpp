#include "ActActionSequenceSectionOverlayController.h"

#include "ActActionTimeSliderController.h"
#include "PlaySlate.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/ActActionSequenceSectionOverlayWidget.h"

FActActionSequenceSectionOverlayController::FActActionSequenceSectionOverlayController(const TSharedRef<FActActionTimeSliderController>& InActActionTimeSliderController)
	: ActActionTimeSliderController(InActActionTimeSliderController)
{
}

FActActionSequenceSectionOverlayController::~FActActionSequenceSectionOverlayController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionSequenceSectionOverlayController::~FActActionSequenceSectionOverlayController"));
}

void FActActionSequenceSectionOverlayController::MakeSequenceSectionOverlayWidget(ActActionSequence::ESectionOverlayWidgetType InOverlayWidgetType)
{
	ActActionSequenceSectionOverlayWidget = SNew(SActActionSequenceSectionOverlayWidget, SharedThis(this))
			.Visibility(EVisibility::HitTestInvisible)
			.DisplayScrubPosition(InOverlayWidgetType == ActActionSequence::ESectionOverlayWidgetType::ScrubPosition)
			.DisplayTickLines(InOverlayWidgetType == ActActionSequence::ESectionOverlayWidgetType::TickLines)
			.DisplayMarkedFrames(InOverlayWidgetType == ActActionSequence::ESectionOverlayWidgetType::ScrubPosition)
			.Clipping(EWidgetClipping::ClipToBounds);
}

void FActActionSequenceSectionOverlayController::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionDrawTickArgs& InArgs) const
{
	if (ActActionTimeSliderController.IsValid())
	{
		ActActionTimeSliderController.Pin()->DrawTicks(OutDrawElements, ViewRange, RangeToScreen, InArgs);
	}
}

ActActionSequence::FActActionTimeSliderArgs& FActActionSequenceSectionOverlayController::GetTimeSliderArgs() const
{
	return ActActionTimeSliderController.Pin()->GetTimeSliderArgs();
}
