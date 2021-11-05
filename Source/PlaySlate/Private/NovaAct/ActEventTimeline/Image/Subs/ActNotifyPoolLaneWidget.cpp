#include "ActNotifyPoolLaneWidget.h"

#include "ActNotifyPoolNotifyNodeWidget.h"
#include "SCurveEditor.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"

using namespace NovaStruct;

void SActNotifyPoolLaneWidget::Construct(const FArguments& InArgs)
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

int32 SActNotifyPoolLaneWidget::OnPaint(const FPaintArgs& Args,
                                        const FGeometry& AllottedGeometry,
                                        const FSlateRect& MyCullingRect,
                                        FSlateWindowElementList& OutDrawElements,
                                        int32 LayerId,
                                        const FWidgetStyle& InWidgetStyle,
                                        bool bParentEnabled) const
{
	int32 CustomLayerId = LayerId + 1;
	auto AnimSequenceDB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!AnimSequenceDB)
	{
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, CustomLayerId, InWidgetStyle, bParentEnabled);
	}

	bool bAnyDraggedNodes = false;
	float AllottedGeometrySizeX = AllottedGeometry.Size.X;
	for (int32 Index = 0; Index < NotifyNodes.Num(); ++Index)
	{
		TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyWidget = NotifyNodes[Index];
		if (NotifyWidget->bBeingDragged)
		{
			bAnyDraggedNodes = true;
		}
		else
		{
			NotifyWidget->UpdateSizeAndPosition(AllottedGeometrySizeX);
		}
	}
	UAnimSequenceBase* AnimSequence = *(AnimSequenceDB->GetData());

	if (TrackIndex < AnimSequence->AnimNotifyTracks.Num())
	{
		// Draw track bottom border
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			CustomLayerId,
			AllottedGeometry.ToPaintGeometry(),
			TArray<FVector2D>({
				FVector2D(0.0f, AllottedGeometry.GetLocalSize().Y),
				FVector2D(AllottedGeometry.GetLocalSize().X, AllottedGeometry.GetLocalSize().Y)
			}),
			ESlateDrawEffect::None,
			FLinearColor::White.CopyWithNewOpacity(0.3f)
		);
	}

	++CustomLayerId;

	auto CurrentDraggedNodePosDB = GetDataBinding(float, "PoolWidgetNotifyWidget/CurrentDraggedNodePos");
	if (bAnyDraggedNodes && !CurrentDraggedNodePosDB)
	{
		float Value = CurrentDraggedNodePosDB->GetData();
		if (Value >= 0.0f)
		{
			float XPos = Value;
			TArray<FVector2D> LinePoints;
			LinePoints.Add(FVector2D(XPos, 0.f));
			LinePoints.Add(FVector2D(XPos, AllottedGeometry.Size.Y));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				CustomLayerId,
				AllottedGeometry.ToPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				FLinearColor(1.0f, 0.5f, 0.0f)
			);
		}
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, CustomLayerId, InWidgetStyle, bParentEnabled);
}

void SActNotifyPoolLaneWidget::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	if (!DB)
	{
		return;
	}
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
	CachedScaleInfo = MakeShareable(new FTrackScaleInfo(ActEventTimelineArgs->ViewRange->GetLowerBoundValue(),
	                                                    ActEventTimelineArgs->ViewRange->GetUpperBoundValue(),
	                                                    0,
	                                                    0,
	                                                    AllottedGeometry.Size));
	CachedAllottedGeometrySizeScaled = AllottedGeometry.Size * AllottedGeometry.Scale;
}

void SActNotifyPoolLaneWidget::Update()
{
	// NotifyPairs.Empty();
	NotifyNodes.Empty();

	TrackBorder->SetContent(
		SAssignNew(NodeSlots, SOverlay)
	);

	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	UAnimSequence* AnimSequence = *(DB->GetData());
	FAnimNotifyTrack& AnimNotifyTrack = AnimSequence->AnimNotifyTracks[TrackIndex];
	TArray<FAnimNotifyEvent*>& Notifies = AnimNotifyTrack.Notifies;
	if (Notifies.Num() > 0)
	{
		// TArray<TSharedPtr<FTimingRelevantElementBase>> TimingElements;
		// SAnimTimingPanel::GetTimingRelevantElements(AnimSequence, TimingElements);
		for (int32 NotifyIndex = 0; NotifyIndex < Notifies.Num(); ++NotifyIndex)
		{
			// TSharedPtr<FTimingRelevantElementBase> Element;
			FAnimNotifyEvent* AnimNotifyEvent = Notifies[NotifyIndex];

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
			// 		if (Event == &Sequence->Notify[NotifyElement->NotifyIndex])
			// 		{
			// 			break;
			// 		}
			// 	}
			// }

			TSharedPtr<SActNotifyPoolNotifyNodeWidget> AnimNotifyNode = nullptr;
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
			// 			return &(StateElement->Sequence->Notify[StateElement->NotifyIndex]) == Event;
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

			AnimNotifyNode = SNew(SActNotifyPoolNotifyNodeWidget, SharedThis(this))
				.AnimNotifyEvent(AnimNotifyEvent);
			// .Sequence(Sequence)
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
			                                                                                     &SActNotifyPoolLaneWidget::GetNotifyTrackPadding,
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
FMargin SActNotifyPoolLaneWidget::GetNotifyTrackPadding(int32 NotifyIndex) const
{
	float LeftMargin = NotifyNodes[NotifyIndex]->GetWidgetPosition().X;
	float RightMargin = GetCachedGeometry().GetLocalSize().X - NotifyNodes[NotifyIndex]->GetWidgetPosition().X -
		NotifyNodes[NotifyIndex]->GetWidgetSize().X;
	return FMargin(LeftMargin, 0, RightMargin, 0);
}

FText SActNotifyPoolLaneWidget::GetNodeTooltip()
{
	return FText();
}

float SActNotifyPoolLaneWidget::GetPlayLength()
{
	// FPlaySlateModule& PlaySlateModule = FModuleManager::GetModuleChecked<FPlaySlateModule>("PlaySlate");
	// ** TODO;
	// return PlaySlateModule.NovaActEditor.Pin()->GetActActionSequence()->AnimSequence->GetPlayLength();
	return 0;
}

FName SActNotifyPoolLaneWidget::GetName()
{
	return NAME_None;
}


FLinearColor SActNotifyPoolLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}


bool SActNotifyPoolLaneWidget::IsBranchingPoint()
{
	return true;
}

// void SActNotifyPoolLaneWidget::DeselectAllNotifyNodes(bool bUpdateSelectionSet)
// {
// 	for (TSharedPtr<SActNotifyPoolNotifyNodeWidget> Node : NotifyNodes)
// 	{
// 		Node->bSelected = false;
// 	}
// 	// SelectedNodeIndices.Empty();
//
// 	// if(bUpdateSelectionSet)
// 	// {
// 	// 	SendSelectionChanged();
// 	// }
// }


void SActNotifyPoolLaneWidget::DisconnectSelectedNodesForDrag(TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>>& DragNodes)
{
	for (auto& NotifyNode : NotifyNodes)
	{
		if (NotifyNode->IsSelected())
		{
			NodeSlots->RemoveSlot(NotifyNode->AsShared());
			DragNodes.Add(NotifyNode);
		}
	}
}
