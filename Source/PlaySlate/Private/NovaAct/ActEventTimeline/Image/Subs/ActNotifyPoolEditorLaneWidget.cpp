#include "ActNotifyPoolEditorLaneWidget.h"

#include "Common/NovaDataBinding.h"

void SActNotifyPoolEditorLaneWidget::Construct(const FArguments& InArgs)
{
	// Sequence = InArgs._Sequence;
	LaneIndex = InArgs._LaneIndex;

	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	
	FAnimNotifyTrack& Track = AnimSequenceBase->AnimNotifyTracks[LaneIndex];
	// @Todo anim: we need to fix this to allow track color to be customizable. 
	// for now name, and track color are given
	Track.TrackColor = ((LaneIndex & 1) != 0) ? FLinearColor(0.9f, 0.9f, 0.9f, 0.9f) : FLinearColor(0.5f, 0.5f, 0.5f);

	// TSharedRef<SAnimNotifyPanel> PanelRef = InArgs._AnimNotifyPanel.ToSharedRef();
	// AnimPanelPtr = InArgs._AnimNotifyPanel;

	//////////////////////////////
	ChildSlot
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		.FillWidth(1)
		[
			// Notification editor panel
			SAssignNew(NotifyTrack, SActNotifyPoolLaneWidget)
			.LaneIndex(LaneIndex)
			// .Sequence(Sequence)
			// .AnimNotifies(Track.Notifies)
			// .AnimSyncMarkers(Track.SyncMarkers)
			// .ViewInputMin(InArgs._ViewInputMin)
			// .ViewInputMax(InArgs._ViewInputMax)
			// .OnSelectionChanged(InArgs._OnSelectionChanged)
			// .OnUpdatePanel(InArgs._OnUpdatePanel)
			// .OnGetNotifyBlueprintData(InArgs._OnGetNotifyBlueprintData)
			// .OnGetNotifyStateBlueprintData(InArgs._OnGetNotifyStateBlueprintData)
			// .OnGetNotifyNativeClasses(InArgs._OnGetNotifyNativeClasses)
			// .OnGetNotifyStateNativeClasses(InArgs._OnGetNotifyStateNativeClasses)
			// .OnGetScrubValue(InArgs._OnGetScrubValue)
			// .OnGetDraggedNodePos(InArgs._OnGetDraggedNodePos)
			// .OnNodeDragStarted(InArgs._OnNodeDragStarted)
			// .OnNotifyStateHandleBeingDragged(InArgs._OnNotifyStateHandleBeingDragged)
			// .OnSnapPosition(InArgs._OnSnapPosition)
			// .TrackColor(Track.TrackColor)
			// .OnRequestTrackPan(FPanTrackRequest::CreateSP(PanelRef, &SAnimNotifyPanel::PanInputViewRange))
			// .OnRequestOffsetRefresh(InArgs._OnRequestRefreshOffsets)
			// .OnDeleteNotify(InArgs._OnDeleteNotify)
			// .OnGetIsAnimNotifySelectionValidForReplacement(PanelRef, &SAnimNotifyPanel::IsNotifySelectionValidForReplacement)
			// .OnReplaceSelectedWithNotify(PanelRef, &SAnimNotifyPanel::OnReplaceSelectedWithNotify)
			// .OnReplaceSelectedWithBlueprintNotify(PanelRef, &SAnimNotifyPanel::OnReplaceSelectedWithNotifyBlueprint)
			// .OnDeselectAllNotifies(InArgs._OnDeselectAllNotifies)
			// .OnCopyNodes(InArgs._OnCopyNodes)
			// .OnPasteNodes(InArgs._OnPasteNodes)
			// .OnSetInputViewRange(InArgs._OnSetInputViewRange)
			// .OnGetTimingNodeVisibility(InArgs._OnGetTimingNodeVisibility)
			// .OnInvokeTab(InArgs._OnInvokeTab)
			// .CommandList(PanelRef->GetCommandList())
		]
	];
}
