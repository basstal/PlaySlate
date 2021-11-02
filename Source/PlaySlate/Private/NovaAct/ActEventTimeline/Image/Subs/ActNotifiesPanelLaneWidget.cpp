#include "ActNotifiesPanelLaneWidget.h"

#include "ActActionSequenceNotifyNode.h"
#include "Common/NovaDataBinding.h"

void SActNotifiesPanelLaneWidget::Construct(const FArguments& InArgs)
{
	SetClipping(EWidgetClipping::ClipToBounds);
	TrackIndex = InArgs._TrackIndex;

	// WeakCommandList = InArgs._CommandList;
	// Sequence = InArgs._Sequence;
	// ViewInputMin = InArgs._ViewInputMin;
	// ViewInputMax = InArgs._ViewInputMax;
	// OnSelectionChanged = InArgs._OnSelectionChanged;
	// AnimNotifies = InArgs._AnimNotifies;
	// AnimSyncMarkers = InArgs._AnimSyncMarkers;
	// OnUpdatePanel = InArgs._OnUpdatePanel;
	// OnGetNotifyBlueprintData = InArgs._OnGetNotifyBlueprintData;
	// OnGetNotifyStateBlueprintData = InArgs._OnGetNotifyStateBlueprintData;
	// OnGetNotifyNativeClasses = InArgs._OnGetNotifyNativeClasses;
	// OnGetNotifyStateNativeClasses = InArgs._OnGetNotifyStateNativeClasses;
	// OnGetScrubValue = InArgs._OnGetScrubValue;
	// OnGetDraggedNodePos = InArgs._OnGetDraggedNodePos;
	// OnNodeDragStarted = InArgs._OnNodeDragStarted;
	// OnNotifyStateHandleBeingDragged = InArgs._OnNotifyStateHandleBeingDragged;
	// TrackColor = InArgs._TrackColor;
	// OnSnapPosition = InArgs._OnSnapPosition;
	// OnRequestTrackPan = InArgs._OnRequestTrackPan;
	// OnRequestRefreshOffsets = InArgs._OnRequestOffsetRefresh;
	// OnDeleteNotify = InArgs._OnDeleteNotify;
	// OnGetIsAnimNotifySelectionValidforReplacement = InArgs._OnGetIsAnimNotifySelectionValidForReplacement;
	// OnReplaceSelectedWithNotify = InArgs._OnReplaceSelectedWithNotify;
	// OnReplaceSelectedWithBlueprintNotify = InArgs._OnReplaceSelectedWithBlueprintNotify;
	// OnDeselectAllNotifies = InArgs._OnDeselectAllNotifies;
	// OnCopyNodes = InArgs._OnCopyNodes;
	// OnPasteNodes = InArgs._OnPasteNodes;
	// OnSetInputViewRange = InArgs._OnSetInputViewRange;
	// OnGetTimingNodeVisibility = InArgs._OnGetTimingNodeVisibility;
	// OnInvokeTab = InArgs._OnInvokeTab;

	ChildSlot
	[
		SAssignNew(TrackBorder, SBorder)
			.Visibility(EVisibility::SelfHitTestInvisible)
			.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			.Padding(FMargin(0.f, 0.f))
	];

	Update();
}

void SActNotifiesPanelLaneWidget::Update()
{
	// NotifyPairs.Empty();
	NotifyNodes.Empty();

	TrackBorder->SetContent(
		SAssignNew(NodeSlots, SOverlay)
	);

	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	UAnimSequence* AnimSequence = *(DB->GetData());
	FAnimNotifyTrack& Track = AnimSequence->AnimNotifyTracks[TrackIndex];
	TArray<FAnimNotifyEvent*>& AnimNotifies = Track.Notifies;
	if (AnimNotifies.Num() > 0)
	{
		// TArray<TSharedPtr<FTimingRelevantElementBase>> TimingElements;
		// SAnimTimingPanel::GetTimingRelevantElements(AnimSequence, TimingElements);
		for (int32 NotifyIndex = 0; NotifyIndex < AnimNotifies.Num(); ++NotifyIndex)
		{
			// TSharedPtr<FTimingRelevantElementBase> Element;
			FAnimNotifyEvent* Event = AnimNotifies[NotifyIndex];

			// for (int32 Idx = 0; Idx < TimingElements.Num(); ++Idx)
			// {
			// 	Element = TimingElements[Idx];
			//
			// 	if (Element->GetType() == ETimingElementType::NotifyStateBegin
			// 		|| Element->GetType() == ETimingElementType::BranchPointNotify
			// 		|| Element->GetType() == ETimingElementType::QueuedNotify)
			// 	{
			// 		// Only the notify type will return the type flags above
			// 		FTimingRelevantElement_Notify* NotifyElement = static_cast<FTimingRelevantElement_Notify*>(Element.Get());
			// 		if (Event == &Sequence->Notifies[NotifyElement->NotifyIndex])
			// 		{
			// 			break;
			// 		}
			// 	}
			// }

			TSharedPtr<SActActionSequenceNotifyNode> AnimNotifyNode = nullptr;
			// TSharedPtr<SAnimNotifyPair> NotifyPair = nullptr;
			// TSharedPtr<SAnimTimingNode> TimingNode = nullptr;
			// TSharedPtr<SAnimTimingNode> EndTimingNode = nullptr;

			// // Create visibility attribute to control timing node visibility for notifies
			// TAttribute<EVisibility> TimingNodeVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateLambda(
			// 	[this]()
			// 	{
			// 		if (OnGetTimingNodeVisibility.IsBound())
			// 		{
			// 			return OnGetTimingNodeVisibility.Execute(ETimingElementType::QueuedNotify);
			// 		}
			// 		return EVisibility(EVisibility::Hidden);
			// 	}));
			//
			// SAssignNew(TimingNode, SAnimTimingNode)
			// 	.InElement(Element)
			// 	.bUseTooltip(true)
			// 	.Visibility(TimingNodeVisibility);

			// if (Event->NotifyStateClass)
			// {
			// 	TSharedPtr<FTimingRelevantElementBase>* FoundStateEndElement = TimingElements.FindByPredicate([Event](TSharedPtr<FTimingRelevantElementBase>& ElementToTest)
			// 	{
			// 		if (ElementToTest.IsValid() && ElementToTest->GetType() == ETimingElementType::NotifyStateEnd)
			// 		{
			// 			FTimingRelevantElement_NotifyStateEnd* StateElement = static_cast<FTimingRelevantElement_NotifyStateEnd*>(ElementToTest.Get());
			// 			return &(StateElement->Sequence->Notifies[StateElement->NotifyIndex]) == Event;
			// 		}
			// 		return false;
			// 	});
			//
			// 	if (FoundStateEndElement)
			// 	{
			// 		// Create an end timing node if we have a state
			// 		SAssignNew(EndTimingNode, SAnimTimingNode)
			// 			.InElement(*FoundStateEndElement)
			// 			.bUseTooltip(true)
			// 			.Visibility(TimingNodeVisibility);
			// 	}
			// }

			AnimNotifyNode = SNew(SActActionSequenceNotifyNode, SharedThis(this));
			// .Sequence(Sequence)
			// .AnimNotify(Event)
			// .OnNodeDragStarted(this, &SAnimNotifyTrack::OnNotifyNodeDragStarted, NotifyIndex)
			// .OnNotifyStateHandleBeingDragged(OnNotifyStateHandleBeingDragged)
			// .OnUpdatePanel(OnUpdatePanel)
			// .PanTrackRequest(OnRequestTrackPan)
			// .ViewInputMin(ViewInputMin)
			// .ViewInputMax(ViewInputMax)
			// .OnSnapPosition(OnSnapPosition)
			// .OnSelectionChanged(OnSelectionChanged)
			// .StateEndTimingNode(EndTimingNode);

			// SAssignNew(NotifyPair, SAnimNotifyPair)
			// 	.LeftContent()
			// 	[
			// 		TimingNode.ToSharedRef()
			// 	]
			// 	.Node(AnimNotifyNode);

			NodeSlots->AddSlot()
			         .Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this,
			                                                                                     &SActNotifiesPanelLaneWidget::GetNotifyTrackPadding,
			                                                                                     NotifyIndex)))
			[
				AnimNotifyNode.ToSharedRef()
			];

			NotifyNodes.Add(AnimNotifyNode);
			// NotifyPairs.Add(NotifyPair);
		}
	}

	// for (FAnimSyncMarker* SyncMarker : AnimSyncMarkers)
	// {
	// 	TSharedPtr<SAnimNotifyNode> AnimSyncMarkerNode = nullptr;
	// 	TSharedPtr<SAnimTimingNode> EndTimingNode = nullptr;
	//
	// 	const int32 NodeIndex = NotifyNodes.Num();
	// 	SAssignNew(AnimSyncMarkerNode, SAnimNotifyNode)
	// 		.Sequence(Sequence)
	// 		.AnimSyncMarker(SyncMarker)
	// 		.OnNodeDragStarted(this, &SAnimNotifyTrack::OnNotifyNodeDragStarted, NodeIndex)
	// 		.OnUpdatePanel(OnUpdatePanel)
	// 		.PanTrackRequest(OnRequestTrackPan)
	// 		.ViewInputMin(ViewInputMin)
	// 		.ViewInputMax(ViewInputMax)
	// 		.OnSnapPosition(OnSnapPosition)
	// 		.OnSelectionChanged(OnSelectionChanged)
	// 		.StateEndTimingNode(EndTimingNode);
	//
	// 	NodeSlots->AddSlot()
	// 	         .Padding(TAttribute<FMargin>::Create(TAttribute<FMargin>::FGetter::CreateSP(this, &SAnimNotifyTrack::GetSyncMarkerTrackPadding, NodeIndex)))
	// 	[
	// 		AnimSyncMarkerNode->AsShared()
	// 	];
	//
	// 	NotifyNodes.Add(AnimSyncMarkerNode);
	// }
}

// Returns the padding needed to render the notify in the correct track position
FMargin SActNotifiesPanelLaneWidget::GetNotifyTrackPadding(int32 NotifyIndex) const
{
	float LeftMargin = 0;
	float RightMargin = CachedGeometry.GetLocalSize().X - NotifyNodes[NotifyIndex]->GetWidgetPosition().X - NotifyNodes[NotifyIndex]->GetWidgetSize().X;
	return FMargin(LeftMargin, 0, RightMargin, 0);
}

FText SActNotifiesPanelLaneWidget::GetNodeTooltip()
{
	return FText();
}

float SActNotifiesPanelLaneWidget::GetPlayLength()
{
	// FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName SActNotifiesPanelLaneWidget::GetName()
{
	return NAME_None;
}


FLinearColor SActNotifiesPanelLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}


bool SActNotifiesPanelLaneWidget::IsBranchingPoint()
{
	return true;
}