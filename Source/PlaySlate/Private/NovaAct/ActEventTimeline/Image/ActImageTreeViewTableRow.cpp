#include "ActImageTreeViewTableRow.h"

#include "ActActionSequenceSectionBase.h"
// #include "ActImageAreaPanel.h"
#include "ActImageTrackPanel.h"
#include "Animation/AnimMontage.h"
#include "Common/NovaConst.h"

#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"

#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeView.h"
// #include "Subs/NovaActUICommandInfo.h"
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackFolder.h"
#include "ImageTrackTypes/ActImageTrackNotify.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaConst;

SActImageTreeViewTableRow::SActImageTreeViewTableRow()
	: TableRowType(ENovaTreeViewTableRowType::None),
	  ActActionTrackAreaArgs(),
	  CachedHitBox(nullptr),
	  Height(0),
	  PendingRenameTrackIndex(0) { }

void SActImageTreeViewTableRow::Construct(const FArguments& InArgs,
                                          const TSharedRef<STableViewBase>& OwnerTableView,
                                          FName InNodeName,
                                          ENovaTreeViewTableRowType InNodeType)
{
	NodeName = InNodeName;
	TableRowType = InNodeType;
	switch (TableRowType)
	{
	case ENovaTreeViewTableRowType::None: break;
	case ENovaTreeViewTableRowType::Folder:
		{
			ActImageTrack = MakeShared<FActImageTrackFolder>();
			break;
		}
	case ENovaTreeViewTableRowType::Notify:
		{
			ActImageTrack = MakeShared<FActImageTrackNotify>();
			break;
		}
	default: ;
	}
	FArguments MultiColumnTableRowArgs;
	{
		// MultiColumnTableRowArgs._OnDragDetected.BindRaw(this, &SActActionSequenceTreeViewRow::OnDragDetected);
		// MultiColumnTableRowArgs._OnCanAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnCanAcceptDrop);
		// MultiColumnTableRowArgs._OnAcceptDrop.BindRaw(this, &SActActionSequenceTreeViewRow::OnAcceptDrop);
		MultiColumnTableRowArgs._ShowSelection = true;
		// MultiColumnTableRowArgs._Padding.BindRaw(this, &SActActionSequenceTreeViewRow::GetRowPadding);
	}

	SMultiColumnTableRow::Construct(InArgs, OwnerTableView);
}

TSharedRef<SWidget> SActImageTreeViewTableRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	if (!ActImageTrack.IsValid())
	{
		return SNullWidget::NullWidget;
	}
	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			ActImageTrack->GenerateContentWidgetForTableRow(SharedThis(this))
		];
}

void SActImageTreeViewTableRow::HandleNotifyChanged()
{
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");

	SetHeight((float)ActAnimationDB->GetData()->AnimSequence->AnimNotifyTracks.Num() * NovaConst::NotifyHeight);
	// RefreshNotifiesPanelTableRow();
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


ENovaTreeViewTableRowType SActImageTreeViewTableRow::GetTableRowType() const
{
	return TableRowType;
}

TSharedRef<IActImageTrackBase> SActImageTreeViewTableRow::GetActImageTrack() const
{
	return ActImageTrack.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
