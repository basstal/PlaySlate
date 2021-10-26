#include "ActTreeViewHorizontalBox.h"

// #include "PlaySlate.h"
#include "Common/NovaConst.h"
// #include "Common/NovaStaticFunction.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"
// #include "NovaAct/Assets/ActActionSequenceStructs.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
// #include "NovaAct/ActEventTimeline/TreeView/ActActionSequenceSectionBase.h"
// #include "NovaAct/Widgets/ActEventTimeline/TreeView/ActActionOutlinerTreeNode.h"
#include "NovaAct/Widgets/ActEventTimeline/TreeView/ActActionSequenceTreeView.h"
#include "NovaAct/Widgets/ActEventTimeline/TreeView/ActActionSequenceTrackArea.h"
// #include "NovaAct/Widgets/ActEventTimeline/TreeView/ActTrackPanel.h"

#include "Widgets/Layout/SScrollBorder.h"
// #include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "NovaAct"

void SActTreeViewHorizontalBox::Construct(const FArguments& InArgs)
{
	const TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	const TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar).Thickness(FVector2D(9.0f, 9.0f));
	TrackArea = SNew(SActActionSequenceTrackArea);
	TreeView = SNew(SActActionSequenceTreeView, TrackArea.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds);
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(TreeView.Get(), &SActActionSequenceTreeView::OnHitBoxesChanged));


	TrackAreaPinned = SNew(SActActionSequenceTrackArea);
	TreeViewPinned = SNew(SActActionSequenceTreeView, TrackAreaPinned.ToSharedRef())
		.ExternalScrollbar(PinnedAreaScrollBar)
		.Clipping(EWidgetClipping::ClipToBounds);


	auto FillLeftAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return DB->GetData();
	}));
	auto FillRightAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return 1 - DB->GetData();
	}));

	Slot()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SScrollBorder, TreeView.ToSharedRef())
			[
				SNew(SHorizontalBox)

				// outliner tree
				+ SHorizontalBox::Slot()
				.FillWidth(FillLeftAttr)
				[
					SNew(SBox)
					[
						TreeView.ToSharedRef()
					]
				]

				// track area
				+ SHorizontalBox::Slot()
				.FillWidth(FillRightAttr)
				[
					SNew(SBox)
				.Padding(NovaConst::ResizeBarPadding)
				.Clipping(EWidgetClipping::ClipToBounds)
					[
						TrackArea.ToSharedRef()
					]
				]
			]
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		[
			ScrollBar
		]
	];
}

//
// TSharedRef<SWidget> SActTreeViewHorizontalBox::MakeOutlinerWidget(const TSharedRef<SActActionSequenceTreeViewRow>& InRow)
// {
// 	TSharedRef<SWidget> Widget =
// 		SNew(SHorizontalBox)
// 		// .ToolTipText(this, &FAnimTimelineTrack::GetToolTipText)
// 		+ SHorizontalBox::Slot()
// 		[
// 			SAssignNew(OutlinerWidget, SVerticalBox)
// 		];
//
// 	RefreshOutlinerWidget();
//
// 	return Widget;
// 	// ActActionOutlinerTreeNode = SNew(SActActionOutlinerTreeNode, SharedThis(this), InRow)
// 	// .IconBrush(this, &SActTreeViewHorizontalBox::GetIconBrush)
// 	// .IconColor(this, &SActTreeViewHorizontalBox::GetDisplayNameColor)
// 	// .IconOverlayBrush(this, &SActTreeViewHorizontalBox::GetIconOverlayBrush)
// 	// .IconToolTipText(this, &SActTreeViewHorizontalBox::GetIconToolTipText)
// 	// .CustomContent()
// 	// [
// 	// 	OutlinerContent ? OutlinerContent.ToSharedRef() : SNullWidget::NullWidget
// 	// ];
// 	// return ActActionOutlinerTreeNode.ToSharedRef();
// }
//
// TSharedRef<SActTrackPanel> SActTreeViewHorizontalBox::GetActTrackPanel()
// {
// 	if (!ActTrackPanel.IsValid())
// 	{
// 		// UAnimMontage* AnimMontage = Cast<UAnimMontage>();
// 		// bool bChildAnimMontage = AnimMontage && AnimMontage->HasParentAsset();
//
// 		auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
//
// 		ActTrackPanel = SNew(SActTrackPanel)
// 			// .IsEnabled(!bChildAnimMontage)
// 			.Sequence(ActAnimationDB->GetData()->AnimSequence);
// 		// .InputMin(this, &FAnimTimelineTrack_NotifiesPanel::GetMinInput)
// 		// .InputMax(this, &FAnimTimelineTrack_NotifiesPanel::GetMaxInput)
// 		// .ViewInputMin(this, &FAnimTimelineTrack_NotifiesPanel::GetViewMinInput)
// 		// .ViewInputMax(this, &FAnimTimelineTrack_NotifiesPanel::GetViewMaxInput);
// 		// .OnGetScrubValue(this, &FAnimTimelineTrack_NotifiesPanel::GetScrubValue)
// 		// .OnSelectionChanged(this, &FAnimTimelineTrack_NotifiesPanel::SelectObjects)
// 		// .OnSetInputViewRange(this, &FAnimTimelineTrack_NotifiesPanel::OnSetInputViewRange)
// 		// .OnInvokeTab(GetModel()->OnInvokeTab)
// 		// .OnSnapPosition(&GetModel().Get(), &FAnimModel::Snap)
// 		// .OnGetTimingNodeVisibility(this, &FAnimTimelineTrack_NotifiesPanel::OnGetTimingNodeVisibility)
// 		// .OnNotifiesChanged_Lambda([this]()
// 		//                                    {
// 		//                                     Update();
// 		//                                     GetModel()->OnTracksChanged().Broadcast();
// 		//
// 		//                                     if (StatusBarMessageHandle.IsValid())
// 		//                                     {
// 		// 	                                    if (UStatusBarSubsystem* StatusBarSubsystem = GEditor->GetEditorSubsystem<UStatusBarSubsystem>())
// 		// 	                                    {
// 		// 		                                    StatusBarSubsystem->PopStatusBarMessage(AnimationEditorStatusBarName, StatusBarMessageHandle);
// 		// 		                                    StatusBarMessageHandle.Reset();
// 		// 	                                    }
// 		//                                     }
// 		//                                    })
// 		// .OnNotifyStateHandleBeingDragged_Lambda([this](TSharedPtr<SAnimNotifyNode> NotifyNode, const FPointerEvent& Event, ENotifyStateHandleHit::Type Handle, float Time)
// 		//                                    {
// 		//                                     if (Event.IsShiftDown())
// 		//                                     {
// 		// 	                                    const FFrameTime FrameTime = FFrameTime::FromDecimal(Time * (double)GetModel()->GetTickResolution());
// 		// 	                                    GetModel()->SetScrubPosition(FrameTime);
// 		//                                     }
// 		//
// 		//                                     if (!StatusBarMessageHandle.IsValid())
// 		//                                     {
// 		// 	                                    if (UStatusBarSubsystem* StatusBarSubsystem = GEditor->GetEditorSubsystem<UStatusBarSubsystem>())
// 		// 	                                    {
// 		// 		                                    StatusBarMessageHandle = StatusBarSubsystem->PushStatusBarMessage(AnimationEditorStatusBarName,
// 		// 		                                                                                                      LOCTEXT("AutoscrubNotifyStateHandle", "Hold SHIFT while dragging a notify state Begin or End handle to auto scrub the timeline."));
// 		// 	                                    }
// 		//                                     }
// 		//                                    })
// 		// .OnNotifyNodesBeingDragged_Lambda([this](const TArray<TSharedPtr<SAnimNotifyNode>>& NotifyNodes, const class FDragDropEvent& Event, float DragXPosition, float DragTime)
// 		//                                    {
// 		//                                     if (Event.IsShiftDown())
// 		//                                     {
// 		// 	                                    const FFrameTime FrameTime = FFrameTime::FromDecimal(DragTime * (double)GetModel()->GetTickResolution());
// 		// 	                                    GetModel()->SetScrubPosition(FrameTime);
// 		//                                     }
// 		//
// 		//                                     if (!StatusBarMessageHandle.IsValid())
// 		//                                     {
// 		// 	                                    if (UStatusBarSubsystem* StatusBarSubsystem = GEditor->GetEditorSubsystem<UStatusBarSubsystem>())
// 		// 	                                    {
// 		// 		                                    StatusBarMessageHandle = StatusBarSubsystem->PushStatusBarMessage(AnimationEditorStatusBarName,
// 		// 		                                                                                                      LOCTEXT("AutoscrubNotify", "Hold SHIFT while dragging a notify to auto scrub the timeline."));
// 		// 	                                    }
// 		//                                     }
// 		//                                    });
//
// 		// GetModel()->GetAnimSequenceBase()->RegisterOnNotifyChanged(UAnimSequenceBase::FOnNotifyChanged::CreateSP(this, &FAnimTimelineTrack_NotifiesPanel::HandleNotifyChanged));
// 	}
//
// 	return ActTrackPanel.ToSharedRef();
// }


// void SActTreeViewHorizontalBox::HandleNotifyChanged()
// {
// 	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
//
// 	SetHeight((float)ActAnimationDB->GetData()->AnimSequence->AnimNotifyTracks.Num() * NovaConst::NotifyHeight);
// 	RefreshOutlinerWidget();
// }
//
// void SActTreeViewHorizontalBox::RefreshOutlinerWidget()
// {
// 	// OutlinerWidget->ClearChildren();
// 	//
// 	// int32 TrackIndex = 0;
// 	// auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
// 	//
// 	// UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;
// 	// // for (FAnimNotifyTrack& AnimNotifyTrack : AnimSequence->AnimNotifyTracks)
// 	// // {
// 	// // 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
// 	// // 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
// 	// // 	return ActEventTimelineArgs->ViewRange.GetLowerBoundValue();
// 	// // };
// 	// ActActionTrackAreaArgs.ViewInputMax.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
// 	// {
// 	// 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
// 	// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
// 	// 	return ActEventTimelineArgs->ViewRange.GetUpperBoundValue();
// 	// }));
// 	// auto TickResolutionLambda = TAttribute<FFrameRate>::FGetter::CreateLambda([this]()
// 	// {
// 	// 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
// 	// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
// 	// 	return ActEventTimelineArgs->TickResolution;
// 	// });
// 	// ActActionTrackAreaArgs.TickResolution.Bind(TickResolutionLambda);
// 	// ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
// 	// 	.Visibility(EVisibility::Visible)
// 	// 	.TickResolution(ActActionTrackAreaArgs.TickResolution);
// 	// ActActionTrackAreaSlot = MakeShareable(new FActActionTrackAreaSlot(SharedThis(this)));
// 	// ActActionTrackAreaSlot->MakeTrackLane();
// 	// TSharedPtr<SBox> SlotBox;
// 	// TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;
// 	//
// 	// OutlinerWidget->AddSlot()
// 	//               .AutoHeight()
// 	// [
// 	// 	SAssignNew(SlotBox, SBox)
// 	// 	.HeightOverride(NovaConst::NotifyHeight)
// 	// ];
// 	//
// 	// TSharedPtr<SHorizontalBox> HorizontalBox;
// 	// SlotBox->SetContent(
// 	// 	SNew(SBorder)
// 	// 		.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
// 	// 		.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
// 	// 	[
// 	// 		SAssignNew(HorizontalBox, SHorizontalBox)
// 	// 		+ SHorizontalBox::Slot()
// 	// 		  .FillWidth(1.0f)
// 	// 		  .VAlign(VAlign_Center)
// 	// 		  .HAlign(HAlign_Left)
// 	// 		  .Padding(30.0f, 0.0f, 0.0f, 0.0f)
// 	// 		[
// 	// 			SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
// 	// 				.Text_Lambda([TrackIndex, AnimSequence]()
// 	// 			                                                             {
// 	// 				                                                             if (AnimSequence->AnimNotifyTracks.Num() > 0 && AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndex))
// 	// 				                                                             {
// 	// 					                                                             return FText::FromName(AnimSequence->AnimNotifyTracks[TrackIndex].TrackName);
// 	// 				                                                             }
// 	// 				                                                             return FText::GetEmpty();
// 	// 			                                                             })
// 	// 				.IsSelected(FIsSelected::CreateLambda([]() { return true; }))
// 	// 				.OnTextCommitted(this, &SActTreeViewHorizontalBox::OnCommitTrackName, TrackIndex)
// 	// 		]
// 	//
// 	// 	]
// 	// );
// 	//
// 	// // UAnimMontage* AnimMontage = Cast<UAnimMontage>(GetModel()->GetAnimSequenceBase());
// 	// // if (!(AnimMontage && AnimMontage->HasParentAsset()))
// 	// // {
// 	// // 	HorizontalBox->AddSlot()
// 	// // 	             .AutoWidth()
// 	// // 	             .VAlign(VAlign_Center)
// 	// // 	             .HAlign(HAlign_Right)
// 	// // 	             .Padding(NovaConst::OutlinerRightPadding, 1.0f)
// 	// // 	[
// 	// // 		NovaStaticFunction::MakeTrackButton(LOCTEXT("AddTrackButtonText", "Track"), FOnGetContent::CreateSP(this, &SActTreeViewHorizontalBox::BuildNotifiesPanelSubMenu, TrackIndex), MakeAttributeSP(SlotBox.Get(), &SWidget::IsHovered))
// 	// // 	];
// 	// // }
// 	//
// 	// if (PendingRenameTrackIndex == TrackIndex)
// 	// {
// 	// 	TWeakPtr<SInlineEditableTextBlock> WeakInlineEditableTextBlock = InlineEditableTextBlock;
// 	// 	InlineEditableTextBlock->RegisterActiveTimer(0.0f, FWidgetActiveTimerDelegate::CreateSP(this, &SActTreeViewHorizontalBox::HandlePendingRenameTimer, WeakInlineEditableTextBlock));
// 	// }
// 	//
// 	// TrackIndex++;
// }
//
//
// void SActTreeViewHorizontalBox::OnCommitTrackName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndexToName)
// {
// 	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
//
// 	UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;
// 	if (AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndexToName))
// 	{
// 		FScopedTransaction Transaction(FText::Format(LOCTEXT("RenameNotifyTrack", "Rename Notify Track to '{0}'"), InText));
// 		AnimSequence->Modify();
//
// 		FText TrimText = FText::TrimPrecedingAndTrailing(InText);
// 		AnimSequence->AnimNotifyTracks[TrackIndexToName].TrackName = FName(*TrimText.ToString());
// 	}
// }
//
//
// TSharedRef<SWidget> SActTreeViewHorizontalBox::BuildNotifiesPanelSubMenu(int32 InTrackIndex)
// {
// 	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
//
// 	// UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;
//
// 	FMenuBuilder MenuBuilder(true, nullptr);
//
// 	MenuBuilder.BeginSection("NotifyTrack", LOCTEXT("NotifyTrackMenuSection", "Notify Track"));
// 	{
// 		// MenuBuilder.AddMenuEntry(
// 		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetLabel(),
// 		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetDescription(),
// 		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetIcon(),
// 		// 	FUIAction(
// 		// 		FExecuteAction::CreateSP(this, &FAnimTimelineTrack_NotifiesPanel::InsertTrack, InTrackIndex)
// 		// 	)
// 		// );
// 		//
// 		// if (AnimSequence->AnimNotifyTracks.Num() > 1)
// 		// {
// 		// 	MenuBuilder.AddMenuEntry(
// 		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetLabel(),
// 		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetDescription(),
// 		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetIcon(),
// 		// 		FUIAction(
// 		// 			FExecuteAction::CreateSP(this, &FAnimTimelineTrack_NotifiesPanel::RemoveTrack, InTrackIndex)
// 		// 		)
// 		// 	);
// 		// }
// 	}
// 	MenuBuilder.EndSection();
//
// 	return MenuBuilder.MakeWidget();
// }


// EActiveTimerReturnType SActTreeViewHorizontalBox::HandlePendingRenameTimer(double InCurrentTime, float InDeltaTime, TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock)
// {
// 	if (InInlineEditableTextBlock.IsValid())
// 	{
// 		InInlineEditableTextBlock.Pin()->EnterEditingMode();
// 	}
//
// 	PendingRenameTrackIndex = INDEX_NONE;
//
// 	return EActiveTimerReturnType::Stop;
// }
//
// TSharedRef<SActTrackPanel> SActTreeViewHorizontalBox::MakeWidgetForTrackArea()
// {
// 	GetActTrackPanel();
//
// 	// ActTrackPanel->Update();
//
// 	return ActTrackPanel.ToSharedRef();
// }

// bool SActTreeViewHorizontalBox::IsTreeViewRoot() const
// {
// 	return ParentNode == nullptr;
// }


// FString SActTreeViewHorizontalBox::GetPathName() const
// {
// 	// First get our parent's path
// 	FString PathName;
//
// 	const TSharedPtr<SActTreeViewHorizontalBox> Parent = GetParentNode();
// 	if (Parent.IsValid())
// 	{
// 		ensure(Parent != SharedThis(this));
// 		PathName = Parent->GetPathName() + TEXT(".");
// 	}
//
// 	//then append our path
// 	PathName += NodeName.ToString();
//
// 	return PathName;
// }
//
// bool SActTreeViewHorizontalBox::IsHidden() const
// {
// 	return false;
// }
//
// bool SActTreeViewHorizontalBox::IsSelectable() const
// {
// 	return true;
// }
//
// bool SActTreeViewHorizontalBox::IsVisible() const
// {
// 	return true;
// }
//

// TSharedPtr<SActTreeViewHorizontalBox> SActTreeViewHorizontalBox::GetSectionAreaAuthority()
// {
// 	if (IsTreeViewRoot())
// 	{
// 		return nullptr;
// 	}
// 	return SharedThis(this);
// }

//
// TArray<TSharedRef<FActActionSequenceSectionBase>>& SActTreeViewHorizontalBox::GetSections()
// {
// 	return Sections;
// }

// float SActTreeViewHorizontalBox::GetNodeHeight() const
// {
// 	const float SectionHeight = Sections.Num() > 0 ? Sections[0]->GetSectionHeight() : 15.0f;
// 	const float PaddedSectionHeight = SectionHeight + 6.0f;
// 	return PaddedSectionHeight;
// }
//
// bool SActTreeViewHorizontalBox::CanRenameNode() const
// {
// 	return true;
// }
//
// FSlateFontInfo SActTreeViewHorizontalBox::GetDisplayNameFont() const
// {
// 	return FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");
// }
//
// FSlateColor SActTreeViewHorizontalBox::GetDisplayNameColor() const
// {
// 	return FLinearColor(0.6f, 0.6f, 0.6f, 0.6f);
// }
//
// bool SActTreeViewHorizontalBox::ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const
// {
// 	if (NewDisplayName.IsEmpty())
// 	{
// 		OutErrorMessage = NSLOCTEXT("Sequence", "RenameFailed_LeftBlank", "Labels cannot be left blank");
// 		return false;
// 	}
// 	else if (NewDisplayName.ToString().Len() >= NAME_SIZE)
// 	{
// 		OutErrorMessage = FText::Format(NSLOCTEXT("Sequence", "RenameFailed_TooLong", "Names must be less than {0} characters long"), NAME_SIZE);
// 		return false;
// 	}
// 	return true;
// }

// void SActTreeViewHorizontalBox::SetDisplayName(const FText& NewDisplayName)
// {
// 	FText OutErrorMessage;
// 	if (ValidateDisplayName(NewDisplayName, OutErrorMessage))
// 	{
// 		NodeName = FName(NewDisplayName.ToString());
// 	}
// }
//
// const FSlateBrush* SActTreeViewHorizontalBox::GetIconBrush() const
// {
// 	return nullptr;
// }
//
// const FSlateBrush* SActTreeViewHorizontalBox::GetIconOverlayBrush() const
// {
// 	return nullptr;
// }

// FText SActTreeViewHorizontalBox::GetIconToolTipText() const
// {
// 	return FText();
// }

//
// void SActTreeViewHorizontalBox::AddDisplayNode(TSharedPtr<SActTreeViewHorizontalBox> ChildTreeViewNode)
// {
// 	DisplayedRootNodes.Add(ChildTreeViewNode.ToSharedRef());
// 	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
// }
//
// void SActTreeViewHorizontalBox::Refresh()
// {
// 	DisplayedRootNodes.Reset();
// 	for (auto& Item : GetChildNodes())
// 	{
// 		if (Item->IsVisible())
// 		{
// 			DisplayedRootNodes.Add(Item);
// 		}
// 	}
// 	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
// }

// TSharedRef<SActTreeViewHorizontalBox> SActTreeViewHorizontalBox::FindOrCreateFolder(const FName& InName)
// {
// 	TSharedRef<SActTreeViewHorizontalBox>* FindNode = ChildNodes.FindByPredicate([InName](auto ChildNode)
// 	{
// 		return ChildNode->NodeName == InName;
// 	});
// 	if (!FindNode)
// 	{
// 		// TSharedRef<SActTreeViewHorizontalBox> Folder = MakeShareable(new SActTreeViewHorizontalBox(ActActionSequenceController.Pin().ToSharedRef(), InName, ENovaSequenceNodeType::Folder));
// 		// Folder->SetParent(SharedThis(this), 0);
// 		// return Folder;
// 	}
// 	return *FindNode;
// }
//
//
// void SActTreeViewHorizontalBox::SetContentAsHitBox(FActActionHitBoxData& InHitBox)
// {
// 	// ** TODO:临时先把对象存这里
// 	CachedHitBox = &InHitBox;
// 	ActActionTrackAreaArgs.Begin.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
// 	{
// 		return CachedHitBox->Begin;
// 	}));
// 	ActActionTrackAreaArgs.End.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
// 	{
// 		return CachedHitBox->End;
// 	}));
// }

// void SActTreeViewHorizontalBox::SetVisible(EVisibility InVisibility)
// {
// 	// if (ActActionOutlinerTreeNode.IsValid())
// 	// {
// 	// 	ActActionOutlinerTreeNode->SetVisibility(InVisibility);
// 	// }
// 	if (ActActionTrackAreaSlot.IsValid())
// 	{
// 		ActActionTrackAreaSlot->SetVisibility(InVisibility);
// 	}
// }

// float SActTreeViewHorizontalBox::ComputeTrackPosition()
// {
// 	// Positioning strategy:
// 	// Attempt to root out any visible track in the specified track's sub-hierarchy, and compute the track's offset from that
// 	const FGeometry& CachedGeometryOutlinerTreeNode = ActActionOutlinerTreeNode->GetCachedGeometry();
// 	// UE_LOG(LogNovaAct, Log, TEXT("CachedGeometryOutlinerTreeNode : %s"), *CachedGeometryOutlinerTreeNode.ToString());
// 	const FGeometry& CachedGeometryTrackArea = TrackArea->GetCachedGeometry();
// 	return CachedGeometryOutlinerTreeNode.AbsolutePosition.Y - CachedGeometryTrackArea.AbsolutePosition.Y;
// }

//
// TSharedPtr<SActTreeViewHorizontalBox> SActTreeViewHorizontalBox::GetRoot()
// {
// 	if (IsTreeViewRoot())
// 	{
// 		return AsShared();
// 	}
//
// 	TSharedPtr<SActTreeViewHorizontalBox> RootNode = AsShared();
// 	while (!RootNode->IsTreeViewRoot())
// 	{
// 		RootNode = RootNode->GetParentNode();
// 	}
// 	return RootNode.ToSharedRef();
// }

// EVisibility SActTreeViewHorizontalBox::GetVisibility() const
// {
// 	if (ActActionOutlinerTreeNode.IsValid())
// 	{
// 		return ActActionOutlinerTreeNode->GetVisibility();
// 	}
// }
#undef LOCTEXT_NAMESPACE
