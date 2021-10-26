#include "ActActionTrackAreaSlot.h"

#include "ActActionSequenceTreeViewNode.h"
#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/Widgets/ActEventTimeline/TreeView/Subs/ActActionSequenceTrackLane.h"
#include "Widgets/SWeakWidget.h"


FActActionTrackAreaSlot::FActActionTrackAreaSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InSequenceTreeViewNode)
	: HAlignment(HAlign_Fill),
	  VAlignment(VAlign_Top),
	  SequenceTreeViewNode(InSequenceTreeViewNode)
// AnimNotifyEvent(nullptr)
{}

void FActActionTrackAreaSlot::MakeTrackLane()
{
	TrackLane = SNew(SActActionSequenceTrackLane, SharedThis(this))
	[
		SequenceTreeViewNode.Pin()->GetActActionSectionWidget()
	];
	TSharedRef<SWeakWidget> AttachedWidget = SNew(SWeakWidget)
	.Clipping(EWidgetClipping::ClipToBounds)
	.PossiblyNullContent(TrackLane);
	AttachWidget(AttachedWidget);
}

float FActActionTrackAreaSlot::GetVerticalOffset() const
{
	return TrackLane.IsValid() ? TrackLane->GetPhysicalPosition() : 0.f;
}

FText FActActionTrackAreaSlot::GetNodeTooltip()
{
	return FText();
}

float FActActionTrackAreaSlot::GetPlayLength()
{
	FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName FActActionTrackAreaSlot::GetName()
{
	return NAME_None;
}

FLinearColor FActActionTrackAreaSlot::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

//
// void FActActionTrackAreaSlot::GetTime(float& OutTime, int& OutFrame)
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
// void FActActionTrackAreaSlot::GetDuration(float& OutTime, int& OutFrame)
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

bool FActActionTrackAreaSlot::IsBranchingPoint()
{
	return true;
}

bool FActActionTrackAreaSlot::HasNotifyNode()
{
	ENovaSequenceNodeType NodeType = SequenceTreeViewNode.Pin()->GetType();
	return NodeType != ENovaSequenceNodeType::Root && NodeType != ENovaSequenceNodeType::Folder;
}

void FActActionTrackAreaSlot::SetVisibility(EVisibility InVisibility)
{
	TrackLane->SetVisibility(InVisibility);
}
