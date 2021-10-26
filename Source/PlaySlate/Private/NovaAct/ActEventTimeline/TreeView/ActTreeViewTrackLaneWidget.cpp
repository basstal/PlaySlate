#include "ActTreeViewTrackLaneWidget.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewNode.h"
#include "NovaAct/ActEventTimeline/TreeView/Subs/ActTreeViewTrackCarWidget.h"

#include "Widgets/SWeakWidget.h"


SActTreeViewTrackLaneWidget::SActTreeViewTrackLaneWidget(const TSharedRef<SActTreeViewNode>& InSequenceTreeViewNode)
	: HAlignment(HAlign_Fill),
	  VAlignment(VAlign_Top),
	  SequenceTreeViewNode(InSequenceTreeViewNode)
// AnimNotifyEvent(nullptr)
{}

void SActTreeViewTrackLaneWidget::Construct(const FArguments& InArgs)
{ }

void SActTreeViewTrackLaneWidget::MakeTrackLane()
{
	TrackLane = SNew(SActTreeViewTrackCarWidget)
	[
		SequenceTreeViewNode.Pin()->GetActActionSectionWidget()
	];
	TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
	.Clipping(EWidgetClipping::ClipToBounds)
	.PossiblyNullContent(TrackLane);
	AttachWidget(AttachedWidget);
}

float SActTreeViewTrackLaneWidget::GetVerticalOffset() const
{
	return TrackLane.IsValid() ? TrackLane->GetPhysicalPosition() : 0.f;
}

FText SActTreeViewTrackLaneWidget::GetNodeTooltip()
{
	return FText();
}

float SActTreeViewTrackLaneWidget::GetPlayLength()
{
	FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName SActTreeViewTrackLaneWidget::GetName()
{
	return NAME_None;
}

FLinearColor SActTreeViewTrackLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

//
// void SActTreeViewTrackLaneWidget::GetTime(float& OutTime, int& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = 0;
// 	OutFrame = 0;
// 	if (SequenceTreeViewNode.Pin()->GetType() == ENovaSequenceNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)(TrackAreaArgs.Begin.Get() * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.Begin.Get());
// 	}
// }
//
// void SActTreeViewTrackLaneWidget::GetDuration(float& OutTime, int& OutFrame)
// {
// 	const ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = GetActActionTrackAreaArgs();
// 	const FFrameRate& FrameRate = TrackAreaArgs.TickResolution.Get();
// 	OutTime = NovaConst::ActMinimumNotifyStateFrame * FrameRate.AsInterval();
// 	OutFrame = NovaConst::ActMinimumNotifyStateFrame;
// 	if (SequenceTreeViewNode.Pin()->GetType() == ENovaSequenceNodeType::State)
// 	{
// 		OutTime = FMath::Max(OutTime, (float)((TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * FrameRate.AsInterval()));
// 		OutFrame = FMath::Max(OutFrame, TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get());
// 	}
// }

bool SActTreeViewTrackLaneWidget::IsBranchingPoint()
{
	return true;
}

bool SActTreeViewTrackLaneWidget::HasNotifyNode()
{
	ENovaSequenceNodeType NodeType = SequenceTreeViewNode.Pin()->GetType();
	return NodeType != ENovaSequenceNodeType::Root && NodeType != ENovaSequenceNodeType::Folder;
}

void SActTreeViewTrackLaneWidget::SetVisibility(EVisibility InVisibility)
{
	TrackLane->SetVisibility(InVisibility);
}
