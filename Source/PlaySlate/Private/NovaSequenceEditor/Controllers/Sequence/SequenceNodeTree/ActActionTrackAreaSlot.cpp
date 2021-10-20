#include "ActActionTrackAreaSlot.h"

#include "ActActionSequenceTreeViewNode.h"
#include "PlaySlate.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/Subs/ActActionSequenceTrackLane.h"
#include "Widgets/SWeakWidget.h"


FActActionTrackAreaSlot::FActActionTrackAreaSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InSequenceTreeViewNode)
	: HAlignment(HAlign_Fill),
	  VAlignment(VAlign_Top),
	  SequenceTreeViewNode(InSequenceTreeViewNode)
// AnimNotifyEvent(nullptr)
{
}

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
	return PlaySlateModule.ActActionSequenceEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
}

FName FActActionTrackAreaSlot::GetName()
{
	return NAME_None;
}

FLinearColor FActActionTrackAreaSlot::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

float FActActionTrackAreaSlot::GetTime()
{
	return 0;
}

float FActActionTrackAreaSlot::GetDuration()
{
	return 0;
}

bool FActActionTrackAreaSlot::IsBranchingPoint()
{
	return true;
}
