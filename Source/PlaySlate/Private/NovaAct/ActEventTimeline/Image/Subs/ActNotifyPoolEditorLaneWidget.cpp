#include "ActNotifyPoolEditorLaneWidget.h"

#include "Common/NovaDataBinding.h"

void SActNotifyPoolEditorLaneWidget::Construct(const FArguments& InArgs)
{
	LaneIndex = InArgs._LaneIndex;

	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());

	FAnimNotifyTrack& Track = AnimSequenceBase->AnimNotifyTracks[LaneIndex];
	// @Todo anim: we need to fix this to allow track color to be customizable. 
	// for now name, and track color are given
	Track.TrackColor = ((LaneIndex & 1) != 0) ? FLinearColor(0.9f, 0.9f, 0.9f, 0.9f) : FLinearColor(0.5f, 0.5f, 0.5f);

	ChildSlot
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SAssignNew(NotifyTrack, SActNotifyPoolLaneWidget)
			.LaneIndex(LaneIndex)
		]
	];
}
