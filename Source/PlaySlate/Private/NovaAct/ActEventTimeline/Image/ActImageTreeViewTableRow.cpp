#include "ActImageTreeViewTableRow.h"

#include "PlaySlate.h"
#include "ActActionSequenceSectionBase.h"

#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"

#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/ActEventTimeline/Image/ActActionOutlinerTreeNode.h"
// #include "NovaAct/ActEventTimeline/Image/ActActionSequenceCombinedKeysTrack.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeView.h"
// #include "NovaAct/ActEventTimeline/Image/ActTreeViewTrackAreaPanel.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "NovaAct"


void SActImageTreeViewTableRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, FName InNodeName, ENovaTreeViewNodeType InNodeType)
{
	NodeName = InNodeName;
	NodeType = InNodeType;
	FArguments MultiColumnTableRowArgs;
	{
		// MultiColumnTableRowArgs._OnDragDetected.BindRaw(this, &SActActionSequenceTreeViewRow::OnDragDetected);
		// MultiColumnTableRowArgs._OnCanAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnCanAcceptDrop);
		// MultiColumnTableRowArgs._OnAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnAcceptDrop);
		MultiColumnTableRowArgs._ShowSelection = IsSelectable();
		// MultiColumnTableRowArgs._Padding.BindRaw(this, &SActActionSequenceTreeViewRow::GetRowPadding);
	}

	if (NodeType == ENovaTreeViewNodeType::Folder)
	{
		OutlinerContent = SNew(SBorder)
		.ToolTipText(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]()
		                               {
			                               FFormatNamedArguments Args;
			                               {
				                               Args.Add("NodeName", FText::FromString(*this->NodeName.ToString()));
			                               }
			                               return FText::Format(LOCTEXT("FolderToolTipText", "ToolTip {NodeName}"), Args);
		                               })))
		.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
		.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
		[
			SNew(SHorizontalBox)

			+ SHorizontalBox::Slot()
			  .VAlign(VAlign_Center)
			  .HAlign(HAlign_Left)
			  .Padding(2.0f, 1.0f)
			  .FillWidth(1.0f)
			[
				SNew(STextBlock)
					.TextStyle(&FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("AnimTimeline.Outliner.Label"))
					.Text(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]()
				                {
					                FFormatNamedArguments Args;
					                {
						                Args.Add("NodeName", FText::FromString(*this->NodeName.ToString()));
					                }
					                return FText::Format(LOCTEXT("FolderTextBlock", "{NodeName}"), Args);
				                })))
			]
		];
	}
	else
	{
		OutlinerContent == SNullWidget::NullWidget;
	}

	SMultiColumnTableRow::Construct(InArgs, OwnerTableView);
}


TSharedRef<SWidget> SActImageTreeViewTableRow::MakeOutlinerWidget()
{
	// TSharedRef<SWidget> Widget =
	// 	SNew(SHorizontalBox)
	// 	// .ToolTipText(this, &FAnimTimelineTrack::GetToolTipText)
	// 	+ SHorizontalBox::Slot()
	// 	[
	// 		SAssignNew(OutlinerWidget, SVerticalBox)
	// 	];
	//
	// RefreshOutlinerWidget();
	//
	// return Widget;
	// ActActionOutlinerTreeNode = SNew(SActActionOutlinerTreeNode, SharedThis(this), InRow)
	// .IconBrush(this, &SActImageTreeViewTableRow::GetIconBrush)
	// .IconColor(this, &SActImageTreeViewTableRow::GetDisplayNameColor)
	// .IconOverlayBrush(this, &SActImageTreeViewTableRow::GetIconOverlayBrush)
	// .IconToolTipText(this, &SActImageTreeViewTableRow::GetIconToolTipText)
	// .CustomContent()
	// [
	// 	OutlinerContent ? OutlinerContent.ToSharedRef() : SNullWidget::NullWidget
	// ];
	return OutlinerContent.ToSharedRef();
}

// TSharedRef<SActTrackPanel> SActImageTreeViewTableRow::GetActTrackPanel()
// {
// 	if (!ActTrackPanel.IsValid())
// 	{
// 		// UAnimMontage* AnimMontage = Cast<UAnimMontage>();
// 		// bool bChildAnimMontage = AnimMontage && AnimMontage->HasParentAsset();
//
// 		auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
//
// 		// ActTrackPanel = SNew(SActTrackPanel)
// 		// 	// .IsEnabled(!bChildAnimMontage)
// 		// 	.Sequence(ActAnimationDB->GetData()->AnimSequence);
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


void SActImageTreeViewTableRow::HandleNotifyChanged()
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");

	SetHeight((float)ActAnimationDB->GetData()->AnimSequence->AnimNotifyTracks.Num() * NovaConst::NotifyHeight);
	RefreshOutlinerWidget();
}

void SActImageTreeViewTableRow::RefreshOutlinerWidget()
{
	// OutlinerWidget->ClearChildren();
	//
	// int32 TrackIndex = 0;
	// auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	//
	// UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;
	// // for (FAnimNotifyTrack& AnimNotifyTrack : AnimSequence->AnimNotifyTracks)
	// // {
	// // 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	// // 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	// // 	return ActEventTimelineArgs->ViewRange.GetLowerBoundValue();
	// // };
	// ActActionTrackAreaArgs.ViewInputMax.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
	// {
	// 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	// 	return ActEventTimelineArgs->ViewRange.GetUpperBoundValue();
	// }));
	// auto TickResolutionLambda = TAttribute<FFrameRate>::FGetter::CreateLambda([this]()
	// {
	// 	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	// 	return ActEventTimelineArgs->TickResolution;
	// });
	// ActActionTrackAreaArgs.TickResolution.Bind(TickResolutionLambda);
	// ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
	// 	.Visibility(EVisibility::Visible)
	// 	.TickResolution(ActActionTrackAreaArgs.TickResolution);
	// ActActionTrackAreaSlot = MakeShareable(new SActImageTrackLaneWidget(SharedThis(this)));
	// ActActionTrackAreaSlot->MakeTrackLane();
	// TSharedPtr<SBox> SlotBox;
	// TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;
	//
	// OutlinerWidget->AddSlot()
	//               .AutoHeight()
	// [
	// 	SAssignNew(SlotBox, SBox)
	// 	.HeightOverride(NovaConst::NotifyHeight)
	// ];
	//
	// TSharedPtr<SHorizontalBox> HorizontalBox;
	// SlotBox->SetContent(
	// 	SNew(SBorder)
	// 		.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
	// 		.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
	// 	[
	// 		SAssignNew(HorizontalBox, SHorizontalBox)
	// 		+ SHorizontalBox::Slot()
	// 		  .FillWidth(1.0f)
	// 		  .VAlign(VAlign_Center)
	// 		  .HAlign(HAlign_Left)
	// 		  .Padding(30.0f, 0.0f, 0.0f, 0.0f)
	// 		[
	// 			SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
	// 				.Text_Lambda([TrackIndex, AnimSequence]()
	// 			                                                             {
	// 				                                                             if (AnimSequence->AnimNotifyTracks.Num() > 0 && AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndex))
	// 				                                                             {
	// 					                                                             return FText::FromName(AnimSequence->AnimNotifyTracks[TrackIndex].TrackName);
	// 				                                                             }
	// 				                                                             return FText::GetEmpty();
	// 			                                                             })
	// 				.IsSelected(FIsSelected::CreateLambda([]() { return true; }))
	// 				.OnTextCommitted(this, &SActImageTreeViewTableRow::OnCommitTrackName, TrackIndex)
	// 		]
	//
	// 	]
	// );
	//
	// // UAnimMontage* AnimMontage = Cast<UAnimMontage>(GetModel()->GetAnimSequenceBase());
	// // if (!(AnimMontage && AnimMontage->HasParentAsset()))
	// // {
	// // 	HorizontalBox->AddSlot()
	// // 	             .AutoWidth()
	// // 	             .VAlign(VAlign_Center)
	// // 	             .HAlign(HAlign_Right)
	// // 	             .Padding(NovaConst::OutlinerRightPadding, 1.0f)
	// // 	[
	// // 		NovaStaticFunction::MakeTrackButton(LOCTEXT("AddTrackButtonText", "Track"), FOnGetContent::CreateSP(this, &SActImageTreeViewTableRow::BuildNotifiesPanelSubMenu, TrackIndex), MakeAttributeSP(SlotBox.Get(), &SWidget::IsHovered))
	// // 	];
	// // }
	//
	// if (PendingRenameTrackIndex == TrackIndex)
	// {
	// 	TWeakPtr<SInlineEditableTextBlock> WeakInlineEditableTextBlock = InlineEditableTextBlock;
	// 	InlineEditableTextBlock->RegisterActiveTimer(0.0f, FWidgetActiveTimerDelegate::CreateSP(this, &SActImageTreeViewTableRow::HandlePendingRenameTimer, WeakInlineEditableTextBlock));
	// }
	//
	// TrackIndex++;
}


void SActImageTreeViewTableRow::OnCommitTrackName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndexToName)
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");

	UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;
	if (AnimSequence->AnimNotifyTracks.IsValidIndex(TrackIndexToName))
	{
		FScopedTransaction Transaction(FText::Format(LOCTEXT("RenameNotifyTrack", "Rename Notify Track to '{0}'"), InText));
		AnimSequence->Modify();

		FText TrimText = FText::TrimPrecedingAndTrailing(InText);
		AnimSequence->AnimNotifyTracks[TrackIndexToName].TrackName = FName(*TrimText.ToString());
	}
}


TSharedRef<SWidget> SActImageTreeViewTableRow::BuildNotifiesPanelSubMenu(int32 InTrackIndex)
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");

	UAnimSequenceBase* AnimSequence = ActAnimationDB->GetData()->AnimSequence;

	FMenuBuilder MenuBuilder(true, nullptr);

	MenuBuilder.BeginSection("NotifyTrack", LOCTEXT("NotifyTrackMenuSection", "Notify Track"));
	{
		// MenuBuilder.AddMenuEntry(
		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetLabel(),
		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetDescription(),
		// 	FAnimSequenceTimelineCommands::Get().InsertNotifyTrack->GetIcon(),
		// 	FUIAction(
		// 		FExecuteAction::CreateSP(this, &FAnimTimelineTrack_NotifiesPanel::InsertTrack, InTrackIndex)
		// 	)
		// );
		//
		// if (AnimSequence->AnimNotifyTracks.Num() > 1)
		// {
		// 	MenuBuilder.AddMenuEntry(
		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetLabel(),
		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetDescription(),
		// 		FAnimSequenceTimelineCommands::Get().RemoveNotifyTrack->GetIcon(),
		// 		FUIAction(
		// 			FExecuteAction::CreateSP(this, &FAnimTimelineTrack_NotifiesPanel::RemoveTrack, InTrackIndex)
		// 		)
		// 	);
		// }
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}


EActiveTimerReturnType SActImageTreeViewTableRow::HandlePendingRenameTimer(double InCurrentTime, float InDeltaTime, TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock)
{
	if (InInlineEditableTextBlock.IsValid())
	{
		InInlineEditableTextBlock.Pin()->EnterEditingMode();
	}

	PendingRenameTrackIndex = INDEX_NONE;

	return EActiveTimerReturnType::Stop;
}

//
// TSharedRef<SActTrackPanel> SActImageTreeViewTableRow::MakeWidgetForTrackArea()
// {
// 	// GetActTrackPanel();
//
// 	// ActTrackPanel->Update();
//
// 	return ActTrackPanel.ToSharedRef();
// }

bool SActImageTreeViewTableRow::IsTreeViewRoot() const
{
	return ParentNode == nullptr;
}

const TArray<TSharedRef<SActImageTreeViewTableRow>>& SActImageTreeViewTableRow::GetChildNodes() const
{
	return ChildNodes;
}

TSharedPtr<SActImageTreeViewTableRow> SActImageTreeViewTableRow::GetChildByIndex(int Index) const
{
	if (ChildNodes.Num() > Index)
	{
		return ChildNodes[Index];
	}
	return nullptr;
}

FString SActImageTreeViewTableRow::GetPathName() const
{
	// First get our parent's path
	FString PathName;

	const TSharedPtr<SActImageTreeViewTableRow> Parent = GetParentNode();
	if (Parent.IsValid())
	{
		ensure(Parent != SharedThis(this));
		PathName = Parent->GetPathName() + TEXT(".");
	}

	//then append our path
	PathName += NodeName.ToString();

	return PathName;
}

bool SActImageTreeViewTableRow::IsHidden() const
{
	return false;
}

bool SActImageTreeViewTableRow::IsSelectable() const
{
	return true;
}

bool SActImageTreeViewTableRow::IsVisible() const
{
	return true;
}

ENovaTreeViewNodeType SActImageTreeViewTableRow::GetType() const
{
	return NodeType;
}

void SActImageTreeViewTableRow::SetParent(TSharedPtr<SActImageTreeViewTableRow> InParent, int32 DesiredChildIndex)
{
	if (!InParent || ParentNode == InParent)
	{
		return;
	}
	if (ParentNode)
	{
		// Remove from parent
		ParentNode->ChildNodes.Remove(SharedThis(this));
	}
	// Add to new parent
	if (DesiredChildIndex != INDEX_NONE && ensureMsgf(DesiredChildIndex <= InParent->ChildNodes.Num(), TEXT("Invalid insert index specified")))
	{
		InParent->ChildNodes.Insert(SharedThis(this), DesiredChildIndex);
	}
	else
	{
		InParent->ChildNodes.Add(SharedThis(this));
	}
	ParentNode = InParent;
}


TSharedPtr<SActImageTreeViewTableRow> SActImageTreeViewTableRow::GetSectionAreaAuthority()
{
	if (IsTreeViewRoot())
	{
		return nullptr;
	}
	return SharedThis(this);
}


TArray<TSharedRef<FActActionSequenceSectionBase>>& SActImageTreeViewTableRow::GetSections()
{
	return Sections;
}

float SActImageTreeViewTableRow::GetNodeHeight() const
{
	const float SectionHeight = Sections.Num() > 0 ? Sections[0]->GetSectionHeight() : 15.0f;
	const float PaddedSectionHeight = SectionHeight + 6.0f;
	return PaddedSectionHeight;
}

bool SActImageTreeViewTableRow::CanRenameNode() const
{
	return true;
}

FSlateFontInfo SActImageTreeViewTableRow::GetDisplayNameFont() const
{
	return FEditorStyle::GetFontStyle("Sequencer.AnimationOutliner.RegularFont");
}

FSlateColor SActImageTreeViewTableRow::GetDisplayNameColor() const
{
	return FLinearColor(0.6f, 0.6f, 0.6f, 0.6f);
}

bool SActImageTreeViewTableRow::ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const
{
	if (NewDisplayName.IsEmpty())
	{
		OutErrorMessage = NSLOCTEXT("Sequence", "RenameFailed_LeftBlank", "Labels cannot be left blank");
		return false;
	}
	else if (NewDisplayName.ToString().Len() >= NAME_SIZE)
	{
		OutErrorMessage = FText::Format(NSLOCTEXT("Sequence", "RenameFailed_TooLong", "Names must be less than {0} characters long"), NAME_SIZE);
		return false;
	}
	return true;
}

void SActImageTreeViewTableRow::SetDisplayName(const FText& NewDisplayName)
{
	FText OutErrorMessage;
	if (ValidateDisplayName(NewDisplayName, OutErrorMessage))
	{
		NodeName = FName(NewDisplayName.ToString());
	}
}

const FSlateBrush* SActImageTreeViewTableRow::GetIconBrush() const
{
	return nullptr;
}

const FSlateBrush* SActImageTreeViewTableRow::GetIconOverlayBrush() const
{
	return nullptr;
}

FText SActImageTreeViewTableRow::GetIconToolTipText() const
{
	return FText();
}

void SActImageTreeViewTableRow::AddDisplayNode(TSharedPtr<SActImageTreeViewTableRow> ChildTreeViewNode)
{
	DisplayedRootNodes.Add(ChildTreeViewNode.ToSharedRef());
	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
}

// ** TODO: 绑定到ActAnimation Changed
void SActImageTreeViewTableRow::Refresh()
{
	DisplayedRootNodes.Reset();
	for (auto& Item : GetChildNodes())
	{
		if (Item->IsVisible())
		{
			DisplayedRootNodes.Add(Item);
		}
	}
	TreeView->SetTreeItemsSource(&DisplayedRootNodes);
}

TSharedRef<SActImageTreeViewTableRow> SActImageTreeViewTableRow::FindOrCreateFolder(const FName& InName)
{
	TSharedRef<SActImageTreeViewTableRow>* FindNode = ChildNodes.FindByPredicate([InName](auto ChildNode)
	{
		return ChildNode->NodeName == InName;
	});
	if (!FindNode)
	{
		// ** TODO:
		// TSharedRef<SActImageTreeViewTableRow> Folder = MakeShareable(new SActImageTreeViewTableRow(InName, ENovaTreeViewNodeType::Folder));
		// Folder->SetParent(SharedThis(this), 0);
		// return Folder;
	}
	return *FindNode;
}

void SActImageTreeViewTableRow::SetContentAsHitBox(FActActionHitBoxData& InHitBox)
{
	// ** TODO:临时先把对象存这里
	CachedHitBox = &InHitBox;
	// ActActionTrackAreaArgs.Begin.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
	// {
	// 	return CachedHitBox->Begin;
	// }));
	// ActActionTrackAreaArgs.End.Bind(TAttribute<int>::FGetter::CreateLambda([this]()
	// {
	// 	return CachedHitBox->End;
	// }));
}

void SActImageTreeViewTableRow::SetVisible(EVisibility InVisibility)
{
	// if (ActActionOutlinerTreeNode.IsValid())
	// {
	// 	ActActionOutlinerTreeNode->SetVisibility(InVisibility);
	// }
	// if (ActActionTrackAreaSlot.IsValid())
	// {
	// 	ActActionTrackAreaSlot->SetVisibility(InVisibility);
	// }
}

float SActImageTreeViewTableRow::ComputeTrackPosition()
{
	// ** TODO:
	// Positioning strategy:
	// Attempt to root out any visible track in the specified track's sub-hierarchy, and compute the track's offset from that
	// const FGeometry& CachedGeometryOutlinerTreeNode = ActActionOutlinerTreeNode->GetCachedGeometry();
	// UE_LOG(LogNovaAct, Log, TEXT("CachedGeometryOutlinerTreeNode : %s"), *CachedGeometryOutlinerTreeNode.ToString());
	// const FGeometry& CachedGeometryTrackArea = GetRoot()->ActTreeViewTrackAreaPanel->GetCachedGeometry();
	// return CachedGeometryOutlinerTreeNode.AbsolutePosition.Y - CachedGeometryTrackArea.AbsolutePosition.Y;
	return 0;
}

TSharedRef<SWidget> SActImageTreeViewTableRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<SWidget> ColumnWidget = SNullWidget::NullWidget;
	if (InColumnName == "Outliner")
	{
		ColumnWidget = MakeOutlinerWidget();
	}

	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			ColumnWidget.ToSharedRef()
		];
}

// TSharedPtr<SActImageTreeViewTableRow> SActImageTreeViewTableRow::GetRoot()
// {
// 	if (IsTreeViewRoot())
// 	{
// 		return AsShared();
// 	}
//
// 	TSharedPtr<SActImageTreeViewTableRow> RootNode = AsShared();
// 	while (!RootNode->IsTreeViewRoot())
// 	{
// 		RootNode = RootNode->GetParentNode();
// 	}
// 	return RootNode.ToSharedRef();
// }

// EVisibility SActImageTreeViewTableRow::GetVisibility() const
// {
// 	if (ActActionOutlinerTreeNode.IsValid())
// 	{
// 		return ActActionOutlinerTreeNode->GetVisibility();
// 	}
// }


// TSharedRef<SWidget> SActImageTreeViewTableRow::GenerateWidgetFromColumn(const TSharedRef<SActImageTreeViewTableRow>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row)
// {

// }


#undef LOCTEXT_NAMESPACE
