#include "ActNotifyPoolLaneWidget.h"

#include "ActNotifyPoolNotifyNodeWidget.h"
#include "AssetSelection.h"
#include "BlueprintActionDatabase.h"
#include "IEditableSkeleton.h"
#include "ISkeletonEditorModule.h"
// #include "NovaActUICommandInfo.h"
#include "SCurveEditor.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Common/NovaConst.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetNotifyWidget.h"
#include "NovaAct/ActEventTimeline/Operation/ActNotifyStateClassFilter.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/STextEntryPopup.h"

#define LOCTEXT_NAMESPACE "NovaAct"
using namespace NovaStruct;
using namespace NovaConst;

void SActNotifyPoolLaneWidget::Construct(const FArguments& InArgs)
{
	SetClipping(EWidgetClipping::ClipToBounds);
	LaneIndex = InArgs._LaneIndex;

	// WeakCommandList = InArgs._CommandList;
	// AnimSequence = InArgs._Sequence;
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
	// OnGetIsAnimNotifySelectionValidForReplacement = InArgs._OnGetIsAnimNotifySelectionValidForReplacement;
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
	auto AnimSequenceDB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!AnimSequenceDB)
	{
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 1, InWidgetStyle, bParentEnabled);
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

	if (LaneIndex < AnimSequence->AnimNotifyTracks.Num())
	{
		float LocalSizeY = AllottedGeometry.GetLocalSize().Y;
		// UE_LOG(LogNovaAct, Log, TEXT("LaneIndex %d : LocalSizeY %f"), LaneIndex, LocalSizeY);
		// Draw track bottom border
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(),
			TArray<FVector2D>({
				FVector2D(0.0f, LocalSizeY),
				FVector2D(AllottedGeometry.GetLocalSize().X, LocalSizeY)
			}),
			ESlateDrawEffect::None,
			FLinearColor::White.CopyWithNewOpacity(0.3f)
		);
	}

	auto PoolNotifyDB = GetDataBindingSP(SActPoolWidgetNotifyWidget, "ActPoolNotify");
	if (bAnyDraggedNodes && PoolNotifyDB)
	{
		TSharedPtr<SActPoolWidgetNotifyWidget> ActPoolWidgetNotifyWidget = PoolNotifyDB->GetData();
		float Value = ActPoolWidgetNotifyWidget->CurrentDragXPosition;
		if (Value >= 0.0f)
		{
			float XPos = Value;
			TArray<FVector2D> LinePoints;
			LinePoints.Add(FVector2D(XPos, 0.f));
			LinePoints.Add(FVector2D(XPos, AllottedGeometry.Size.Y));

			FSlateDrawElement::MakeLines(
				OutDrawElements,
				LayerId + 2,
				AllottedGeometry.ToPaintGeometry(),
				LinePoints,
				ESlateDrawEffect::None,
				FLinearColor(1.0f, 0.5f, 0.0f)
			);
		}
	}

	return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId + 2, InWidgetStyle, bParentEnabled);
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

FReply SActNotifyPoolLaneWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();
	CursorPos = MyGeometry.AbsoluteToLocal(CursorPos);
	TSharedPtr<SActNotifyPoolNotifyNodeWidget> HitNotifyNode = GetHitNotifyNode(CursorPos);

	if (HitNotifyNode)
	{
		if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			// Hit a node, record the mouse position for use later so we can know when / where a
			// drag happened on the node handles if necessary.
			HitNotifyNode->LastMouseDownPosition = CursorPos;

			return FReply::Handled().DetectDrag(HitNotifyNode.ToSharedRef(), EKeys::LeftMouseButton);
		}
		else if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// Hit a node, return handled so we can pop a context menu on mouse up
			return FReply::Handled();
		}
	}

	return FReply::Unhandled();
}

FReply SActNotifyPoolLaneWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return FReply::Unhandled();
	}
	bool bLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
	bool bRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
	bool bShift = MouseEvent.IsShiftDown();
	bool bCtrl = MouseEvent.IsControlDown();

	FVector2D MouseAbsolutePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	UAnimSequence* AnimSequence = *DB->GetData();
	LastClickedTime = FMath::Clamp<float>(CachedScaleInfo->LocalXToInput(MouseAbsolutePosition.X), 0.0f, AnimSequence->GetPlayLength());
	TSharedPtr<SActNotifyPoolNotifyNodeWidget> HitNotifyNode = GetHitNotifyNode(MouseAbsolutePosition);

	if (bRightMouseButton)
	{
		TSharedPtr<SWidget> WidgetToFocus = SummonContextMenu(MouseEvent, HitNotifyNode);

		return (WidgetToFocus.IsValid()) ?
			       FReply::Handled().ReleaseMouseCapture().SetUserFocus(WidgetToFocus.ToSharedRef(), EFocusCause::SetDirectly) :
			       FReply::Handled().ReleaseMouseCapture();
	}
	else if (bLeftMouseButton)
	{
		auto PoolNotifyDB = GetDataBindingSP(SActPoolWidgetNotifyWidget, "ActPoolNotify");
		if (!PoolNotifyDB)
		{
			return FReply::Unhandled();
		}
		TSharedPtr<SActPoolWidgetNotifyWidget> ActPoolWidgetNotifyWidget = PoolNotifyDB->GetData();
		if (!HitNotifyNode)
		{
			// Clicked in empty space, clear selection
			ActPoolWidgetNotifyWidget->DeselectAllNotifies();
		}
		else if (bCtrl)
		{
			ActPoolWidgetNotifyWidget->ToggleNotifyNodeSelectStatus(HitNotifyNode.ToSharedRef());
		}
		else
		{
			ActPoolWidgetNotifyWidget->SelectNotifyNode(HitNotifyNode.ToSharedRef(), bShift);
		}

		return FReply::Handled();
	}

	return FReply::Unhandled();
}

FVector2D SActNotifyPoolLaneWidget::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(100, NotifyNodes.IsEmpty() ? NotifyHeight : NotifyNodes[0]->WidgetSize.Y);
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
	FAnimNotifyTrack& AnimNotifyTrack = AnimSequence->AnimNotifyTracks[LaneIndex];
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
			// 		if (Event == &AnimSequence->Notify[NotifyElement->NotifyIndex])
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
			// 			return &(StateElement->AnimSequence->Notify[StateElement->NotifyIndex]) == Event;
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
			// .AnimSequence(AnimSequence)
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
	// 		.AnimSequence(AnimSequence)
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
		NotifyNodes[NotifyIndex]->WidgetSize.X;
	return FMargin(LeftMargin, 0, RightMargin, 0);
}

FText SActNotifyPoolLaneWidget::GetNodeTooltip()
{
	return FText();
}

FName SActNotifyPoolLaneWidget::GetName()
{
	return NAME_None;
}


FLinearColor SActNotifyPoolLaneWidget::GetEditorColor()
{
	return FLinearColor(1, 1, 0.5f);
}

void SActNotifyPoolLaneWidget::DisconnectSelectedNodesForDrag(TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>>& DragNodes)
{
	for (TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyNode : NotifyNodes)
	{
		if (NotifyNode->IsSelected())
		{
			NodeSlots->RemoveSlot(NotifyNode.ToSharedRef());
			DragNodes.Add(NotifyNode);
		}
	}
}

TSharedPtr<SActNotifyPoolNotifyNodeWidget> SActNotifyPoolLaneWidget::GetHitNotifyNode(const FVector2D& CursorPosition)
{
	//Run through from 'top most' Notify to bottom
	for (int32 Index = NotifyNodes.Num() - 1; Index >= 0; --Index)
	{
		TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyNode = NotifyNodes[Index];
		FVector2D Position = NotifyNode->GetWidgetPosition();
		FVector2D Size = NotifyNode->WidgetSize;
		if (CursorPosition >= Position && CursorPosition <= (Position + Size))
		{
			return NotifyNode;
		}
	}

	return nullptr;
}


TSharedPtr<SWidget> SActNotifyPoolLaneWidget::SummonContextMenu(const FPointerEvent& MouseEvent,
                                                                const TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyNode)
{
	auto DB = GetDataBindingSP(FNovaActEditor, "NovaActEditor");
	auto PoolNotifyDB = GetDataBindingSP(SActPoolWidgetNotifyWidget, "ActPoolNotify");
	auto AnimSequenceDB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!DB || !PoolNotifyDB || !AnimSequenceDB)
	{
		return SNullWidget::NullWidget;
	}
	TSharedPtr<SActPoolWidgetNotifyWidget> ActPoolWidgetNotifyWidget = PoolNotifyDB->GetData();
	TSharedPtr<FNovaActEditor> NovaActEditor = DB->GetData();
	FMenuBuilder MenuBuilder(true, NovaActEditor->GetToolkitCommands());
	FUIAction NewAction;

	FAnimNotifyEvent* NotifyEvent = NotifyNode ? NotifyNode->AnimNotifyEvent : nullptr;
	// int32 NotifyIndex = NotifyEvent ? NotifyNodes.IndexOfByKey(NotifyNode) : INDEX_NONE;

	UAnimSequence* AnimSequence = *AnimSequenceDB->GetData();
	MenuBuilder.BeginSection("AnimNotify", LOCTEXT("NotifyHeading", "Notify"));
	{
		if (NotifyNode)
		{
			if (!NotifyNode->IsSelected())
			{
				ActPoolWidgetNotifyWidget->SelectNotifyNode(NotifyNode.ToSharedRef(), MouseEvent.IsControlDown());
			}

			if (ActPoolWidgetNotifyWidget->IsSingleNotifyNodeSelected())
			{
				auto CommitBeginTime = [NotifyEvent, AnimSequenceDB](float InValue, ETextCommit::Type InCommitType, bool bValueAsFrame)
				{
					if (InCommitType == ETextCommit::OnEnter && NotifyEvent && AnimSequenceDB)
					{
						UAnimSequence* InnerAnimSequence = *AnimSequenceDB->GetData();
						float NewTime = FMath::Clamp(bValueAsFrame ? InnerAnimSequence->GetTimeAtFrame(InValue) : InValue,
						                             0.0f,
						                             InnerAnimSequence->GetPlayLength() - NotifyEvent->GetDuration());
						NotifyEvent->SetTime(NewTime);

						NotifyEvent->RefreshTriggerOffset(InnerAnimSequence->CalculateOffsetForNotify(NotifyEvent->GetTime()));
						if (NotifyEvent->GetDuration() > 0.0f)
						{
							NotifyEvent->RefreshEndTriggerOffset(
								InnerAnimSequence->CalculateOffsetForNotify(NotifyEvent->GetTime() + NotifyEvent->GetDuration()));
						}
						NovaDB::Trigger("ActImageTrack/Refresh");

						FSlateApplication::Get().DismissAllMenus();
					}
				};
				// Add item to directly set notify time
				TSharedRef<SWidget> TimeWidget =
					SNew(SBox)
					.HAlign(HAlign_Right)
					.ToolTipText(LOCTEXT("SetTimeToolTip", "Set the time of this notify directly"))
					[
						SNew(SBox)
						.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
						.WidthOverride(100.0f)
						[
							SNew(SNumericEntryBox<float>)
							.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
							.MinValue(0.0f)
							.MaxValue(AnimSequence->GetPlayLength())
							.Value(NotifyEvent->GetTime())
							.AllowSpin(false)
							.OnValueCommitted_Lambda([CommitBeginTime](float InValue, ETextCommit::Type InCommitType)
							{
								CommitBeginTime(InValue, InCommitType, false);
							})
						]
					];

				MenuBuilder.AddWidget(TimeWidget, LOCTEXT("TimeMenuText", "Notify Begin Time"));

				// Add item to directly set notify frame
				TSharedRef<SWidget> FrameWidget =
					SNew(SBox)
					.HAlign(HAlign_Right)
					.ToolTipText(LOCTEXT("SetFrameToolTip", "Set the frame of this notify directly"))
					[
						SNew(SBox)
						.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
						.WidthOverride(100.0f)
						[
							SNew(SNumericEntryBox<int32>)
							.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
							.MinValue(0)
							.MaxValue(AnimSequence->GetNumberOfSampledKeys())
							.Value(AnimSequence->GetFrameAtTime(NotifyEvent->GetTime()))
							.AllowSpin(false)
							.OnValueCommitted_Lambda([CommitBeginTime](int32 InValue, ETextCommit::Type InCommitType)
							{
								CommitBeginTime(InValue, InCommitType, true);
							})
						]
					];

				MenuBuilder.AddWidget(FrameWidget, LOCTEXT("FrameMenuText", "Notify Frame"));

				if (NotifyEvent)
				{
					// add menu to get threshold weight for triggering this notify
					TSharedRef<SWidget> ThresholdWeightWidget =
						SNew(SBox)
						.HAlign(HAlign_Right)
						.ToolTipText(LOCTEXT("MinTriggerWeightToolTip", "The minimum weight to trigger this notify"))
						[
							SNew(SBox)
							.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
							.WidthOverride(100.0f)
							[
								SNew(SNumericEntryBox<float>)
								.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
								.MinValue(0.0f)
								.MaxValue(1.0f)
								.Value(NotifyEvent->TriggerWeightThreshold)
								.AllowSpin(false)
								.OnValueCommitted_Lambda([this, NotifyEvent](float InValue, ETextCommit::Type InCommitType)
								{
									if (InCommitType == ETextCommit::OnEnter && NotifyEvent)
									{
										float NewWeight = FMath::Max(InValue, ZERO_ANIMWEIGHT_THRESH);
										NotifyEvent->TriggerWeightThreshold = NewWeight;

										FSlateApplication::Get().DismissAllMenus();
									}
								})
							]
						];

					auto CommitEndTime = [NotifyEvent, AnimSequenceDB](float InValue, ETextCommit::Type InCommitType, bool bValueAsFrame)
					{
						if (InCommitType == ETextCommit::OnEnter && NotifyEvent && AnimSequenceDB)
						{
							UAnimSequence* AnimSequence = *AnimSequenceDB->GetData();
							float NewDuration = FMath::Max(bValueAsFrame ? AnimSequence->GetTimeAtFrame(InValue) : InValue, MinimumStateDuration);
							float MaxDuration = AnimSequence->GetPlayLength() - NotifyEvent->GetTime();
							NewDuration = FMath::Min(NewDuration, MaxDuration);
							NotifyEvent->SetDuration(NewDuration);

							// // If we have a delegate bound to refresh the offsets, call it.
							// // This is used by the montage editor to keep the offsets up to date.
							// OnRequestRefreshOffsets.ExecuteIfBound();

							FSlateApplication::Get().DismissAllMenus();
						}
					};
					MenuBuilder.AddWidget(ThresholdWeightWidget, LOCTEXT("MinTriggerWeight", "Min Trigger Weight"));

					// Add menu for changing duration if this is an AnimNotifyState
					if (NotifyEvent->NotifyStateClass)
					{
						TSharedRef<SWidget> NotifyStateDurationWidget =
							SNew(SBox)
							.HAlign(HAlign_Right)
							.ToolTipText(LOCTEXT("SetAnimStateDuration_ToolTip", "The duration of this Anim Notify State"))
							[
								SNew(SBox)
								.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
								.WidthOverride(100.0f)
								[
									SNew(SNumericEntryBox<float>)
									.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
									.MinValue(MinimumStateDuration)
									.MinSliderValue(MinimumStateDuration)
									.MaxSliderValue(100.0f)
									.Value(NotifyEvent->GetDuration())
									.AllowSpin(false)
									.OnValueCommitted_Lambda([CommitEndTime](float InValue, ETextCommit::Type InCommitType)
									{
										CommitEndTime(InValue, InCommitType, false);
									})
								]
							];

						MenuBuilder.AddWidget(NotifyStateDurationWidget, LOCTEXT("SetAnimStateDuration", "Anim Notify State Duration"));

						TSharedRef<SWidget> NotifyStateDurationFramesWidget =
							SNew(SBox)
							.HAlign(HAlign_Right)
							.ToolTipText(LOCTEXT("SetAnimStateDurationFrames_ToolTip", "The duration of this Anim Notify State in frames"))
							[
								SNew(SBox)
								.Padding(FMargin(4.0f, 0.0f, 0.0f, 0.0f))
								.WidthOverride(100.0f)
								[
									SNew(SNumericEntryBox<int32>)
									.Font(FEditorStyle::GetFontStyle(TEXT("MenuItem.Font")))
									.MinValue(1)
									.MinSliderValue(1)
									.MaxSliderValue(AnimSequence->GetNumberOfSampledKeys())
									.Value(AnimSequence->GetFrameAtTime(NotifyEvent->GetDuration()))
									.AllowSpin(false)
									.OnValueCommitted_Lambda([CommitEndTime](int32 InValue, ETextCommit::Type InCommitType)
									{
										CommitEndTime(InValue, InCommitType, true);
									})
								]
							];

						MenuBuilder.AddWidget(NotifyStateDurationFramesWidget, LOCTEXT("SetAnimStateDurationFrames", "Anim Notify State Frames"));
					}
				}
			}
		}
		else
		{
			MenuBuilder.AddSubMenu(
				NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuAddNotify", "Add Notify..."),
				NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuAddNotifyToolTip", "Add AnimNotifyEvent"),
				FNewMenuDelegate::CreateRaw(this, &SActNotifyPoolLaneWidget::FillNewNotifyMenu, false));

			MenuBuilder.AddSubMenu(
				NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuAddNotifyState", "Add Notify State..."),
				NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuAddNotifyStateToolTip", "Add AnimNotifyState"),
				FNewMenuDelegate::CreateRaw(this, &SActNotifyPoolLaneWidget::FillNewNotifyStateMenu, false));

			// TODO:?
			// if (AnimSequence->IsA(UAnimSequence::StaticClass()))
			// {
			// 	// MenuBuilder.AddSubMenu(
			// 	// 	NSLOCTEXT("NewSyncMarkerSubMenu", "NewSyncMarkerSubMenuAddNotifyState", "Add Sync Marker..."),
			// 	// 	NSLOCTEXT("NewSyncMarkerSubMenu", "NewSyncMarkerSubMenuAddNotifyStateToolTip", "Create a new animation sync marker"),
			// 	// 	FNewMenuDelegate::CreateRaw(this, &SAnimNotifyTrack::FillNewSyncMarkerMenu));
			// }
			//
			// MenuBuilder.AddMenuEntry(
			// 	NSLOCTEXT("NewNotifySubMenu", "ManageNotifies", "Manage Notifies..."),
			// 	NSLOCTEXT("NewNotifySubMenu", "ManageNotifiesToolTip", "Opens the Manage Notifies window"),
			// 	FSlateIcon(),
			// 	FUIAction(FExecuteAction::CreateSP(this, &SAnimNotifyTrack::OnManageNotifies)));
		}
	}
	MenuBuilder.EndSection();
	//AnimNotify

	NewAction.CanExecuteAction = 0;

	// MenuBuilder.BeginSection("AnimEdit", LOCTEXT("NotifyEditHeading", "Edit"));
	// {
	// 	if (NotifyNode)
	// 	{
	// 		// copy notify menu item
	// 		MenuBuilder.AddMenuEntry(FNovaActNotifiesPanelCommands::Get().CopyNotifies);
	//
	// 		// allow it to delete
	// 		MenuBuilder.AddMenuEntry(FNovaActNotifiesPanelCommands::Get().DeleteNotify);
	//
	// 		if (NotifyEvent)
	// 		{
	// 			// For the "Replace With..." menu, make sure the current AnimNotify selection is valid for replacement
	// 			if (OnGetIsAnimNotifySelectionValidForReplacement.IsBound() && OnGetIsAnimNotifySelectionValidForReplacement.Execute())
	// 			{
	// 				// If this is an AnimNotifyState (has duration) allow it to be replaced with other AnimNotifyStates
	// 				if (NotifyEvent->NotifyStateClass)
	// 				{
	// 					MenuBuilder.AddSubMenu(
	// 						NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuReplaceWithNotifyState", "Replace with Notify State..."),
	// 						NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuReplaceWithNotifyStateToolTip", "Replace with AnimNotifyState"),
	// 						FNewMenuDelegate::CreateRaw(this, &SActNotifyPoolLaneWidget::FillNewNotifyStateMenu, true));
	// 				}
	// 					// If this is a regular AnimNotify (no duration) allow it to be replaced with other AnimNotifies
	// 				else
	// 				{
	// 					MenuBuilder.AddSubMenu(
	// 						NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuReplaceWithNotify", "Replace with Notify..."),
	// 						NSLOCTEXT("NewNotifySubMenu", "NewNotifySubMenuReplaceWithNotifyToolTip", "Replace with AnimNotifyEvent"),
	// 						FNewMenuDelegate::CreateRaw(this, &SActNotifyPoolLaneWidget::FillNewNotifyMenu, true));
	// 				}
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		FString PropertyString;
	// 		const TCHAR* Buffer;
	// 		float OriginalTime;
	// 		float OriginalLength;
	// 		int32 TrackSpan;
	//
	// 		//Check whether can we show menu item to paste anim notify event
	// 		if (ReadNotifyPasteHeader(PropertyString, Buffer, OriginalTime, OriginalLength, TrackSpan))
	// 		{
	// 			// paste notify menu item
	// 			if (IsSingleNodeInClipboard())
	// 			{
	// 				MenuBuilder.AddMenuEntry(FNovaActNotifiesPanelCommands::Get().PasteNotifies);
	// 			}
	// 			else
	// 			{
	// 				NewAction.ExecuteAction.BindRaw(
	// 					this,
	// 					&SActNotifyPoolLaneWidget::OnPasteNotifyClicked,
	// 					ENotifyPasteMode::MousePosition,
	// 					ENotifyPasteMultipleMode::Relative);
	//
	// 				MenuBuilder.AddMenuEntry(
	// 					LOCTEXT("PasteMultRel", "Paste Multiple Relative"),
	// 					LOCTEXT("PasteMultRelToolTip",
	// 					        "Paste multiple notifies beginning at the mouse cursor, maintaining the same relative spacing as the source."),
	// 					FSlateIcon(),
	// 					NewAction);
	//
	// 				MenuBuilder.AddMenuEntry(FNovaActNotifiesPanelCommands::Get().PasteNotifies,
	// 				                         NAME_None,
	// 				                         LOCTEXT("PasteMultAbs", "Paste Multiple Absolute"),
	// 				                         LOCTEXT("PasteMultAbsToolTip",
	// 				                                 "Paste multiple notifies beginning at the mouse cursor, maintaining absolute spacing."));
	// 			}
	//
	// 			if (OriginalTime < AnimSequence->GetPlayLength())
	// 			{
	// 				NewAction.ExecuteAction.BindRaw(
	// 					this,
	// 					&SActNotifyPoolLaneWidget::OnPasteNotifyClicked,
	// 					ENotifyPasteMode::OriginalTime,
	// 					ENotifyPasteMultipleMode::Absolute);
	//
	// 				FText DisplayText = FText::Format(LOCTEXT("PasteAtOriginalTime", "Paste at original time ({0})"), FText::AsNumber(OriginalTime));
	// 				MenuBuilder.AddMenuEntry(DisplayText,
	// 				                         LOCTEXT("PasteAtOriginalTimeToolTip",
	// 				                                 "Paste animation notify event at the time it was set to when it was copied"),
	// 				                         FSlateIcon(),
	// 				                         NewAction);
	// 			}
	// 		}
	// 	}
	// }
	// MenuBuilder.EndSection();//AnimEdit
	//
	// if (NotifyEvent)
	// {
	// 	UObject* NotifyObject = NotifyEvent->Notify;
	// 	NotifyObject = NotifyObject ? NotifyObject : ToRawPtr(NotifyEvent->NotifyStateClass);
	//
	// 	MenuBuilder.BeginSection("ViewSource", LOCTEXT("NotifyViewHeading", "View"));
	//
	// 	if (NotifyObject)
	// 	{
	// 		if (Cast<UBlueprintGeneratedClass>(NotifyObject->GetClass()))
	// 		{
	// 			if (UBlueprint* Blueprint = Cast<UBlueprint>(NotifyObject->GetClass()->ClassGeneratedBy))
	// 			{
	// 				NewAction.ExecuteAction.BindRaw(
	// 					this,
	// 					&SActNotifyPoolLaneWidget::OnOpenNotifySource,
	// 					Blueprint);
	// 				MenuBuilder.AddMenuEntry(
	// 					LOCTEXT("OpenNotifyBlueprint", "Open Notify Blueprint"),
	// 					LOCTEXT("OpenNotifyBlueprintTooltip", "Opens the source blueprint for this notify"),
	// 					FSlateIcon(),
	// 					NewAction);
	// 			}
	// 		}
	// 	}
	// 	else
	// 	{
	// 		// skeleton notify
	// 		NewAction.ExecuteAction.BindRaw(
	// 			this,
	// 			&SActNotifyPoolLaneWidget::OnFilterSkeletonNotify,
	// 			NotifyEvent->NotifyName);
	// 		MenuBuilder.AddMenuEntry(LOCTEXT("FindNotifyReferences", "Find References"),
	// 		                         LOCTEXT("FindNotifyReferencesTooltip", "Find all references to this skeleton notify in the asset browser"),
	// 		                         FSlateIcon(),
	// 		                         NewAction);
	// 	}
	//
	// 	MenuBuilder.EndSection();//ViewSource
	// }

	FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();

	// Display the newly built menu
	FSlateApplication::Get().PushMenu(SharedThis(this),
	                                  WidgetPath,
	                                  MenuBuilder.MakeWidget(),
	                                  MouseEvent.GetScreenSpacePosition(),
	                                  FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));

	return TSharedPtr<SWidget>();
}

void SActNotifyPoolLaneWidget::FillNewNotifyStateMenu(FMenuBuilder& MenuBuilder, bool bIsReplaceWithMenu /* = false */)
{
	auto AnimSequenceDB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!AnimSequenceDB)
	{
		return;
	}
	UAnimSequence* AnimSequence = *AnimSequenceDB->GetData();
	NotifyStateFilter::MakeNewNotifyPicker<UAnimNotifyState>(MenuBuilder, false, AnimSequence);
}

void SActNotifyPoolLaneWidget::FillNewNotifyMenu(FMenuBuilder& MenuBuilder, bool bIsReplaceWithMenu /* = false */)
{
	auto AnimSequenceDB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!AnimSequenceDB)
	{
		return;
	}
	UAnimSequence* AnimSequence = *AnimSequenceDB->GetData();
	// now add custom anim notifiers
	USkeleton* SeqSkeleton = AnimSequence->GetSkeleton();
	if (SeqSkeleton)
	{
		MenuBuilder.BeginSection("AnimNotifySkeletonSubMenu", LOCTEXT("NewNotifySubMenu_Skeleton", "Skeleton Notifies"));
		{
			if (!bIsReplaceWithMenu)
			{
				FUIAction UIAction;
				UIAction.ExecuteAction.BindSP(
					this,
					&SActNotifyPoolLaneWidget::OnNewNotifyClicked);
				MenuBuilder.AddMenuEntry(LOCTEXT("NewNotify", "New Notify..."),
				                         LOCTEXT("NewNotifyToolTip", "Create a new animation notify on the skeleton"),
				                         FSlateIcon(),
				                         UIAction);
			}

			MenuBuilder.AddSubMenu(
				LOCTEXT("NewNotifySubMenu_Skeleton", "Skeleton Notifies"),
				LOCTEXT("NewNotifySubMenu_Skeleton_Tooltip", "Choose from custom notifies on the skeleton"),
				FNewMenuDelegate::CreateLambda([this, SeqSkeleton](FMenuBuilder& InSubMenuBuilder)
				{
					ISkeletonEditorModule& SkeletonEditorModule = FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");
					TSharedRef<IEditableSkeleton> EditableSkeleton = SkeletonEditorModule.CreateEditableSkeleton(SeqSkeleton);

					InSubMenuBuilder.AddWidget(
						SNew(SBox)
						.MinDesiredWidth(300.0f)
						.MaxDesiredHeight(400.0f),
						// TODO:
						// [
						// 	SNew(SSkeletonAnimNotifies, EditableSkeleton)
						// 	.IsPicker(true)
						// 	.OnItemSelected_Lambda([this, bIsReplaceWithMenu](const FName& InNotifyName)
						// 	{
						// 		FSlateApplication::Get().DismissAllMenus();
						// 	
						// 		if (!bIsReplaceWithMenu)
						// 		{
						// 			CreateNewNotifyAtCursor(InNotifyName.ToString(), nullptr);
						// 		}
						// 		else
						// 		{
						// 			ReplaceSelectedWithNotify(InNotifyName.ToString(), nullptr);
						// 		}
						// 	})
						// ],
						FText(),
						true,
						false
					);
				}));
		}
		MenuBuilder.EndSection();
	}

	NotifyStateFilter::MakeNewNotifyPicker<UAnimNotify>(MenuBuilder, bIsReplaceWithMenu, AnimSequence);
}


void SActNotifyPoolLaneWidget::OnNewNotifyClicked()
{
	// Show dialog to enter new track name
	TSharedRef<STextEntryPopup> TextEntry =
		SNew(STextEntryPopup)
		.Label(LOCTEXT("NewNotifyLabel", "Notify Name"))
		.OnTextCommitted(this, &SActNotifyPoolLaneWidget::AddNewNotify);

	// Show dialog to enter new event name
	FSlateApplication::Get().PushMenu(
		AsShared(),
		// Menu being summoned from a menu that is closing: Parent widget should be k2 not the menu that's open or it will be closed when the menu is dismissed
		FWidgetPath(),
		TextEntry,
		FSlateApplication::Get().GetCursorPos(),
		FPopupTransitionEffect(FPopupTransitionEffect::TypeInPopup)
	);
}


void SActNotifyPoolLaneWidget::AddNewNotify(const FText& NewNotifyName, ETextCommit::Type CommitInfo)
{
	auto AnimSequenceDB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!AnimSequenceDB)
	{
		return;
	}
	UAnimSequence* AnimSequence = *AnimSequenceDB->GetData();
	USkeleton* Skeleton = AnimSequence->GetSkeleton();
	if ((CommitInfo == ETextCommit::OnEnter) && Skeleton)
	{
		const FScopedTransaction Transaction(LOCTEXT("AddNewNotifyEvent", "Add New Anim Notify"));
		FName NewName = FName(*NewNotifyName.ToString());

		ISkeletonEditorModule& SkeletonEditorModule = FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");
		TSharedRef<IEditableSkeleton> EditableSkeleton = SkeletonEditorModule.CreateEditableSkeleton(Skeleton);

		EditableSkeleton->AddNotify(NewName);

		FBlueprintActionDatabase::Get().RefreshAssetActions(Skeleton);

		auto DB = GetDataBindingSP(FActCreateNewNotify, "CreateNewNotify");
		TSharedPtr<FActCreateNewNotify> CreateNewNotify = MakeShareable(new FActCreateNewNotify());
		{
			CreateNewNotify->NotifyClass = nullptr;
			CreateNewNotify->NewNotifyName = NewNotifyName.ToString();
		}
		DB->SetData(CreateNewNotify);
	}

	FSlateApplication::Get().DismissAllMenus();
}


void SActNotifyPoolLaneWidget::OnCreateNewNotify(TSharedPtr<FActCreateNewNotify> InActCreateNewNotify)
{
	if (!InActCreateNewNotify)
	{
		return;
	}
	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	const FScopedTransaction Transaction(LOCTEXT("AddNotifyEvent", "Add Anim Notify"));
	UAnimSequence* AnimSequence = *DB->GetData();
	AnimSequence->Modify();
	FAnimNotifyEvent _;
	CreateNewNotify(InActCreateNewNotify->NewNotifyName, InActCreateNewNotify->NotifyClass, LastClickedTime, _);
	NovaDB::Trigger("ActImageTrack/Refresh");
}


bool SActNotifyPoolLaneWidget::CreateNewNotify(FString NewNotifyName, UClass* NotifyClass, float StartTime, FAnimNotifyEvent& OutAnimNotifyEvent)
{
	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return false;
	}
	UAnimSequence* AnimSequence = *DB->GetData();
	// Insert a new notify record and spawn the new notify object
	int32 NewNotifyIndex = AnimSequence->Notifies.Add(FAnimNotifyEvent());
	OutAnimNotifyEvent = AnimSequence->Notifies[NewNotifyIndex];
	OutAnimNotifyEvent.NotifyName = FName(*NewNotifyName);
	OutAnimNotifyEvent.Guid = FGuid::NewGuid();

	OutAnimNotifyEvent.Link(AnimSequence, StartTime);
	OutAnimNotifyEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(AnimSequence->CalculateOffsetForNotify(StartTime));
	OutAnimNotifyEvent.TrackIndex = LaneIndex;

	if (NotifyClass)
	{
		class UObject* AnimNotifyClass = NewObject<UObject>(AnimSequence, NotifyClass, NAME_None, RF_Transactional);
		OutAnimNotifyEvent.NotifyStateClass = Cast<UAnimNotifyState>(AnimNotifyClass);
		OutAnimNotifyEvent.Notify = Cast<UAnimNotify>(AnimNotifyClass);

		if (OutAnimNotifyEvent.NotifyStateClass)
		{
			// Set default duration to 1 frame for AnimNotifyState.
			OutAnimNotifyEvent.SetDuration(MinimumStateDuration);
			OutAnimNotifyEvent.EndLink.Link(AnimSequence, OutAnimNotifyEvent.EndLink.GetTime());
			OutAnimNotifyEvent.TriggerWeightThreshold = OutAnimNotifyEvent.NotifyStateClass->GetDefaultTriggerWeightThreshold();
		}
		else if (OutAnimNotifyEvent.Notify)
		{
			OutAnimNotifyEvent.TriggerWeightThreshold = OutAnimNotifyEvent.Notify->GetDefaultTriggerWeightThreshold();
		}
	}
	else
	{
		OutAnimNotifyEvent.Notify = NULL;
		OutAnimNotifyEvent.NotifyStateClass = NULL;
	}

	if (OutAnimNotifyEvent.Notify)
	{
		TArray<FAssetData> SelectedAssets;
		AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

		for (TFieldIterator<FObjectProperty> PropIt(OutAnimNotifyEvent.Notify->GetClass()); PropIt; ++PropIt)
		{
			if (PropIt->GetBoolMetaData(TEXT("ExposeOnSpawn")))
			{
				FObjectProperty* Property = *PropIt;
				const FAssetData* Asset = SelectedAssets.FindByPredicate([Property](const FAssetData& Other)
				{
					return Other.GetAsset()->IsA(Property->PropertyClass);
				});

				if (Asset)
				{
					uint8* Offset = (*PropIt)->ContainerPtrToValuePtr<uint8>(OutAnimNotifyEvent.Notify);
					(*PropIt)->ImportText(*Asset->GetAsset()->GetPathName(), Offset, 0, OutAnimNotifyEvent.Notify);
					break;
				}
			}
		}

		OutAnimNotifyEvent.Notify->OnAnimNotifyCreatedInEditor(OutAnimNotifyEvent);
	}
	else if (OutAnimNotifyEvent.NotifyStateClass)
	{
		OutAnimNotifyEvent.NotifyStateClass->OnAnimNotifyCreatedInEditor(OutAnimNotifyEvent);
	}

	AnimSequence->PostEditChange();
	AnimSequence->MarkPackageDirty();

	return true;
}

#undef LOCTEXT_NAMESPACE
