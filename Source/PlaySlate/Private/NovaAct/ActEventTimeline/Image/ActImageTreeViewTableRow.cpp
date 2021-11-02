#include "ActImageTreeViewTableRow.h"

#include "ActActionSequenceSectionBase.h"
#include "ActImageAreaPanel.h"
#include "ActImageTrackPanel.h"
#include "Animation/AnimMontage.h"
#include "Common/NovaConst.h"

#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"

#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeView.h"
#include "Subs/NovaActUICommandInfo.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaConst;

void SActImageTreeViewTableRow::Construct(const FArguments& InArgs,
                                          const TSharedRef<STableViewBase>& OwnerTableView,
                                          FName InNodeName,
                                          ENovaTreeViewTableRowType InNodeType)
{
	NodeName = InNodeName;
	TableRowType = InNodeType;
	FArguments MultiColumnTableRowArgs;
	{
		// MultiColumnTableRowArgs._OnDragDetected.BindRaw(this, &SActActionSequenceTreeViewRow::OnDragDetected);
		// MultiColumnTableRowArgs._OnCanAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnCanAcceptDrop);
		// MultiColumnTableRowArgs._OnAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnAcceptDrop);
		MultiColumnTableRowArgs._ShowSelection = IsSelectable();
		// MultiColumnTableRowArgs._Padding.BindRaw(this, &SActActionSequenceTreeViewRow::GetRowPadding);
	}

	SMultiColumnTableRow::Construct(InArgs, OwnerTableView);
}

TSharedRef<SWidget> SActImageTreeViewTableRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	switch (TableRowType)
	{
	case ENovaTreeViewTableRowType::None: break;
	case ENovaTreeViewTableRowType::Folder:
		{
			return SNew(SOverlay)
				+ SOverlay::Slot()
				[
					SNew(SBorder)
					.ToolTipText_Lambda([this]()
					             {
						             FFormatNamedArguments Args;
						             {
							             Args.Add("NodeName", FText::FromString(*this->NodeName.ToString()));
						             }
						             return FText::Format(LOCTEXT("FolderToolTipText", "ToolTip {NodeName}"), Args);
					             })
					.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
					.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
					[
						SNew(SHorizontalBox)

						+ SHorizontalBox::Slot()
						  .VAlign(VAlign_Center)
						  .AutoWidth()
						  .Padding(4.0f, 1.0f)
						[
							SNew(SExpanderArrow, SharedThis(this))
						]

						+ SHorizontalBox::Slot()
						  .VAlign(VAlign_Center)
						  .HAlign(HAlign_Left)
						  .Padding(2.0f, 1.0f)
						  .FillWidth(1.0f)
						[
							SNew(STextBlock)
							.TextStyle(&FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("AnimTimeline.Outliner.Label"))
							.Text_Lambda([this]()
							{
								FFormatNamedArguments Args;
								{
									Args.Add("NodeName", FText::FromString(*this->NodeName.ToString()));
								}
								return FText::Format(LOCTEXT("FolderTextBlock", "{NodeName}"), Args);
							})
						]
					]
				];
		}
	case ENovaTreeViewTableRowType::Notifies:
		{
			auto NotifiesBox = SNew(SHorizontalBox)
				.ToolTipText(FText::GetEmpty())
				+ SHorizontalBox::Slot()
				[
					SAssignNew(NotifiesPanelTableRow, SVerticalBox)
				];

			RefreshNotifiesPanelTableRow();
			return NotifiesBox;
		}
	default: ;
	}
	return SNullWidget::NullWidget;
}

void SActImageTreeViewTableRow::HandleNotifyChanged()
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");

	SetHeight((float)ActAnimationDB->GetData()->AnimSequence->AnimNotifyTracks.Num() * NovaConst::NotifyHeight);
	// RefreshNotifiesPanelTableRow();
}

void SActImageTreeViewTableRow::RefreshNotifiesPanelTableRow()
{
	if (TableRowType != ENovaTreeViewTableRowType::Notifies)
	{
		return;
	}
	NotifiesPanelTableRow->ClearChildren();

	int32 TrackIndex = 0;
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");

	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());
	for (FAnimNotifyTrack& AnimNotifyTrack : AnimSequenceBase->AnimNotifyTracks)
	{
		TSharedPtr<SBox> SlotBox;
		TSharedPtr<SInlineEditableTextBlock> InlineEditableTextBlock;

		NotifiesPanelTableRow->AddSlot()
		                     .AutoHeight()
		[
			SAssignNew(SlotBox, SBox)
			.HeightOverride(NotifyHeight)
		];

		TSharedPtr<SHorizontalBox> HorizontalBox;

		auto TextLambda = [TrackIndex, AnimSequenceBase]()
		{
			return AnimSequenceBase->AnimNotifyTracks.IsValidIndex(TrackIndex) ?
				       FText::FromName(AnimSequenceBase->AnimNotifyTracks[TrackIndex].TrackName) :
				       FText::GetEmpty();
		};
		SlotBox->SetContent(SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("Sequencer.Section.BackgroundTint"))
			.BorderBackgroundColor(FEditorStyle::GetColor("AnimTimeline.Outliner.ItemColor"))
			[
				SAssignNew(HorizontalBox, SHorizontalBox)
				+ SHorizontalBox::Slot()
				  .FillWidth(1.0f)
				  .VAlign(VAlign_Center)
				  .HAlign(HAlign_Left)
				  .Padding(30.0f, 0.0f, 0.0f, 0.0f)
				[
					SAssignNew(InlineEditableTextBlock, SInlineEditableTextBlock)
					.Text_Lambda(TextLambda)
					.IsSelected_Lambda([]() { return true; })
					.OnTextCommitted(this, &SActImageTreeViewTableRow::OnCommitTrackName, TrackIndex)
				]

			]
		);

		UAnimMontage* AnimMontage = Cast<UAnimMontage>(AnimSequenceBase);
		// 非 Montage 资源且有 ParentAsset 的情况下 都可以添加 TrackLane
		if (!(AnimMontage && AnimMontage->HasParentAsset()))
		{
			HorizontalBox->AddSlot()
			             .AutoWidth()
			             .VAlign(VAlign_Center)
			             .HAlign(HAlign_Right)
			             .Padding(OutlinerRightPadding, 1.0f)
			[
				NovaStaticFunction::MakeTrackButton(LOCTEXT("AddTrackButtonText", "Track"),
				                                    FOnGetContent::CreateSP(this,
				                                                            &SActImageTreeViewTableRow::BuildNotifiesPanelSubMenu,
				                                                            TrackIndex),
				                                    MakeAttributeSP(SlotBox.Get(), &SWidget::IsHovered))
			];
		}

		if (PendingRenameTrackIndex == TrackIndex)
		{
			TWeakPtr<SInlineEditableTextBlock> WeakInlineEditableTextBlock = InlineEditableTextBlock;
			InlineEditableTextBlock->RegisterActiveTimer(0.0f,
			                                             FWidgetActiveTimerDelegate::CreateSP(this,
			                                                                                  &SActImageTreeViewTableRow::HandlePendingRenameTimer,
			                                                                                  WeakInlineEditableTextBlock));
		}

		TrackIndex++;
	}
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
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	UAnimSequenceBase* AnimSequenceBase = *(DB->GetData());

	auto HostAppDB = GetDataBindingSP(FNovaActEditor, "NovaActEditor");
	FMenuBuilder MenuBuilder(true, HostAppDB->GetData()->GetToolkitCommands());

	MenuBuilder.BeginSection("NotifyTrack", LOCTEXT("NotifyTrackMenuSection", "Notify Track"));
	{
		// MenuBuilder.AddMenuEntry(
		// 	FNovaActUICommandInfo::Get().InsertNotifyTrack->GetLabel(),
		// 	FNovaActUICommandInfo::Get().InsertNotifyTrack->GetDescription(),
		// 	FNovaActUICommandInfo::Get().InsertNotifyTrack->GetIcon(),
		// 	FUIAction(FExecuteAction::CreateSP(this, &SActImageTreeViewTableRow::InsertTrack, InTrackIndex))
		// );

		if (AnimSequenceBase->AnimNotifyTracks.Num() > 1)
		{
			// MenuBuilder.AddMenuEntry(
			// 	FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetLabel(),
			// 	FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetDescription(),
			// 	FNovaActUICommandInfo::Get().RemoveNotifyTrack->GetIcon(),
			// 	FUIAction(FExecuteAction::CreateSP(this, &SActImageTreeViewTableRow::RemoveTrack, InTrackIndex))
			// );
		}
	}
	MenuBuilder.EndSection();

	return MenuBuilder.MakeWidget();
}


void SActImageTreeViewTableRow::NotifiesPanelInsertTrack(int32 InTrackIndexToInsert)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	UAnimSequenceBase* AnimSequence = *(DB->GetData());

	FScopedTransaction Transaction(LOCTEXT("InsertNotifyTrack", "Insert Notify Track"));
	AnimSequence->Modify();

	// before insert, make sure everything behind is fixed
	for (int32 TrackIndex = InTrackIndexToInsert; TrackIndex < AnimSequence->AnimNotifyTracks.Num(); ++TrackIndex)
	{
		FAnimNotifyTrack& Track = AnimSequence->AnimNotifyTracks[TrackIndex];

		const int32 NewTrackIndex = TrackIndex + 1;

		for (FAnimNotifyEvent* Notify : Track.Notifies)
		{
			// fix notifies indices
			Notify->TrackIndex = NewTrackIndex;
		}

		for (FAnimSyncMarker* SyncMarker : Track.SyncMarkers)
		{
			// fix notifies indices
			SyncMarker->TrackIndex = NewTrackIndex;
		}
	}

	FAnimNotifyTrack NewItem;
	NewItem.TrackName = NovaStaticFunction::GetNewTrackName(AnimSequence);
	NewItem.TrackColor = FLinearColor::White;

	AnimSequence->AnimNotifyTracks.Insert(NewItem, InTrackIndexToInsert);

	// Request a rename on rebuild
	PendingRenameTrackIndex = InTrackIndexToInsert;

	Update();
}

EActiveTimerReturnType SActImageTreeViewTableRow::HandlePendingRenameTimer(double InCurrentTime,
                                                                           float InDeltaTime,
                                                                           TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock)
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

TSharedPtr<SActImageTreeViewTableRow> SActImageTreeViewTableRow::GetChildByIndex(int32 Index) const
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

// ENovaTreeViewTableRowType SActImageTreeViewTableRow::GetType() const
// {
// 	return TableRowType;
// }

void SActImageTreeViewTableRow::RemoveFromParent()
{
	if (ParentNode)
	{
		// Remove from parent
		ParentNode->ChildNodes.Remove(SharedThis(this));
	}
}

void SActImageTreeViewTableRow::SetParent(TSharedPtr<SActImageTreeViewTableRow> InParent, int32 DesiredChildIndex)
{
	if (!InParent || ParentNode == InParent)
	{
		return;
	}
	RemoveFromParent();
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
		// TSharedRef<SActImageTreeViewTableRow> Folder = MakeShareable(new SActImageTreeViewTableRow(InName, ENovaTreeViewTableRowType::Folder));
		// Folder->SetParent(SharedThis(this), 0);
		// return Folder;
	}
	return *FindNode;
}

void SActImageTreeViewTableRow::SetContentAsHitBox(FActActionHitBoxData& InHitBox)
{
	// ** TODO:临时先把对象存这里
	CachedHitBox = &InHitBox;
	// ActActionTrackAreaArgs.Begin.Bind(TAttribute<int32>::FGetter::CreateLambda([this]()
	// {
	// 	return CachedHitBox->Begin;
	// }));
	// ActActionTrackAreaArgs.End.Bind(TAttribute<int32>::FGetter::CreateLambda([this]()
	// {
	// 	return CachedHitBox->End;
	// }));
}

//
// void SActImageTreeViewTableRow::SetVisible(EVisibility InVisibility)
// {
// 	if (OutlinerContent.IsValid())
// 	{
// 		OutlinerContent->SetVisibility(InVisibility);
// 	}
// 	if (TrackArea.IsValid())
// 	{
// 		TrackArea->SetVisibility(InVisibility);
// 	}
// }

float SActImageTreeViewTableRow::ComputeTrackPosition()
{
	// Positioning strategy:
	// Attempt to root out any visible track in the specified track's sub-hierarchy, and compute the track's offset from that
	const FGeometry& CachedGeometryOutlinerTreeNode = GetCachedGeometry();
	// UE_LOG(LogNovaAct, Log, TEXT("CachedGeometryOutlinerTreeNode : %s"), *CachedGeometryOutlinerTreeNode.ToString());
	TSharedPtr<SActImageTreeView> ActImageTreeView = StaticCastSharedPtr<SActImageTreeView>(OwnerTablePtr.Pin());
	const FGeometry& CachedGeometryTrackArea = ActImageTreeView->GetCachedGeometry();
	// UE_LOG(LogNovaAct,
	//        Log,
	//        TEXT("CachedGeometryOutlinerTreeNode.AbsolutePosition.Y : %f, CachedGeometryTrackArea.AbsolutePosition.Y : %f"),
	//        CachedGeometryOutlinerTreeNode.AbsolutePosition.Y,
	//        CachedGeometryTrackArea.AbsolutePosition.Y);
	return CachedGeometryOutlinerTreeNode.AbsolutePosition.Y - CachedGeometryTrackArea.AbsolutePosition.Y;
}


void SActImageTreeViewTableRow::Update()
{
	auto DB = GetDataBinding(UAnimSequence**, "ActAnimation/AnimSequence");
	UAnimSequence* AnimSequence = *(DB->GetData());
	SetHeight((float)(AnimSequence->AnimNotifyTracks.Num() * NotifyHeight));
	RefreshNotifiesPanelTableRow();
	if (ActImageTrackPanel.IsValid())
	{
		ActImageTrackPanel->Update();
	}
}

ENovaTreeViewTableRowType SActImageTreeViewTableRow::GetTableRowType() const
{
	return TableRowType;
}

#undef LOCTEXT_NAMESPACE
