#include "ActImageTrackLaneWidget.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/TreeView/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/TreeView/ActImageTrackCarWidget.h"

#include "Widgets/SWeakWidget.h"

SActImageTrackLaneWidget::Slot::Slot(const TSharedRef<SActImageTrackLaneWidget>& InSlotContent)
{
	SlotContent = InSlotContent;

	HAlignment = HAlign_Fill;
	VAlignment = VAlign_Top;

	AttachWidget(
		SNew(SWeakWidget)
		.Clipping(EWidgetClipping::ClipToBounds)
		.PossiblyNullContent(InSlotContent)
	);
}

float SActImageTrackLaneWidget::Slot::GetVerticalOffset() const
{
	if (SlotContent.IsValid())
	{
		return SlotContent.Pin()->GetPhysicalPosition();
	}
	return 0.0f;
}

void SActImageTrackLaneWidget::Construct(const FArguments& InArgs) {}

// void SActImageTrackLaneWidget::MakeTrackLane()
// {
// 	// TrackLane = SNew(SActImageTrackCarWidget)
// 	// [
// 	// 	SequenceTreeViewNode.Pin()->GetActActionSectionWidget()
// 	// ];
// 	// TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
// 	// .Clipping(EWidgetClipping::ClipToBounds)
// 	// .PossiblyNullContent(TrackLane);
// 	// AttachWidget(AttachedWidget);
// }

FText SActImageTrackLaneWidget::GetNodeTooltip()
{
	return FText();
}

float SActImageTrackLaneWidget::GetPlayLength()
{
	FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName SActImageTrackLaneWidget::GetName()
{
	return NAME_None;
}

FLinearColor SActImageTrackLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

//
// void SActImageTrackLaneWidget::GetTime(float& OutTime, int& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = 0;
// 	OutFrame = 0;
// 	if (SequenceTreeViewNode.Pin()->GetType() == ENovaTreeViewNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)(TrackAreaArgs.Begin.Get() * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.Begin.Get());
// 	}
// }
//
// void SActImageTrackLaneWidget::GetDuration(float& OutTime, int& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = NovaConst::ActMinimumNotifyStateFrame * FrameRate.AsInterval();
// 	OutFrame = NovaConst::ActMinimumNotifyStateFrame;
// 	if (SequenceTreeViewNode.Pin()->GetType() == ENovaTreeViewNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)((TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get());
// 	}
// }

bool SActImageTrackLaneWidget::IsBranchingPoint()
{
	return true;
}

bool SActImageTrackLaneWidget::HasNotifyNode()
{
	ENovaTreeViewNodeType NodeType = SequenceTreeViewNode.Pin()->GetType();
	return NodeType != ENovaTreeViewNodeType::Root && NodeType != ENovaTreeViewNodeType::Folder;
}

// void SActImageTrackLaneWidget::SetVisibility(EVisibility InVisibility)
// {
// 	TrackLane->SetVisibility(InVisibility);
// }


float SActImageTrackLaneWidget::GetPhysicalPosition() const
{
	// if (ActActionTrackAreaSlot.IsValid())
	// {
	// 	// return ActActionTrackAreaSlot.Pin()->GetActActionSequenceTreeViewNode()->ComputeTrackPosition();
	// }
	return 0.0f;
}
