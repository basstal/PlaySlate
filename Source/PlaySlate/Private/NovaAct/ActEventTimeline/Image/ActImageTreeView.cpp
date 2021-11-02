#include "ActImageTreeView.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/Image/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/Image/ActImageAreaPanel.h"

#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"


SActImageTreeView::~SActImageTreeView()
{
	auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	if (DB)
	{
		DB->UnBind(OnHitBoxesChangedHandle);
	}
}

void SActImageTreeView::Construct(const FArguments& InArgs, const TSharedRef<SActImageAreaPanel>& InActImageTrackAreaPanel)
{
	ActImageTrackAreaPanel = InActImageTrackAreaPanel;

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed)
		+ SHeaderRow::Column(FName("Outliner"))
		.FillWidth(1.0f);
	FArguments TreeViewArgs;
	{
		TreeViewArgs._TreeItemsSource = &DisplayedRootNodes;
		TreeViewArgs._SelectionMode = ESelectionMode::Single;
		TreeViewArgs._HeaderRow = HeaderRow;
		TreeViewArgs._HighlightParentNodesForSelection = true;
		TreeViewArgs._AllowInvisibleItemSelection = true;
		TreeViewArgs._Clipping = EWidgetClipping::ClipToBounds;
		TreeViewArgs._OnGetChildren.BindRaw(this, &SActImageTreeView::OnGetChildren);
		TreeViewArgs._OnExpansionChanged.BindRaw(this, &SActImageTreeView::OnExpansionChanged);
		TreeViewArgs._ExternalScrollbar = InArgs._ExternalScrollbar;
		TreeViewArgs._OnGenerateRow.BindRaw(this, &SActImageTreeView::OnGenerateRow);
	}
	STreeView::Construct(TreeViewArgs);

	DataBindingUObjectBindRaw(UActAnimation, "ActAnimation", this, &SActImageTreeView::OnHitBoxesChanged, OnHitBoxesChangedHandle);
}

void SActImageTreeView::OnGetChildren(TSharedRef<SActImageTreeViewTableRow> InParent, TArray<TSharedRef<SActImageTreeViewTableRow>>& OutChildren) const
{
	for (const auto& Node : InParent->GetChildNodes())
	{
		if (!Node->IsHidden())
		{
			OutChildren.Add(Node);
		}
	}
}

void SActImageTreeView::OnExpansionChanged(TSharedRef<SActImageTreeViewTableRow> InDisplayNode, bool bIsExpanded)
{
	UE_LOG(LogNovaAct, Log, TEXT("InDisplayNode->GetPathName : %s, bIsExpanded : %d"), *InDisplayNode->GetPathName(), bIsExpanded);
	for (const TSharedRef<SActImageTreeViewTableRow>& ChildNode : InDisplayNode->GetChildNodes())
	{
		TWeakPtr<SActImageTrackLaneWidget>* WeakTrackLanePtr = TreeViewTableRow2TrackLaneWidget.Find(ChildNode);
		if (WeakTrackLanePtr && WeakTrackLanePtr->IsValid())
		{
			TreeViewTableRow2TrackLaneWidget.FindChecked(ChildNode).Pin()->SetVisibility(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
		}
	}
}


void SActImageTreeView::Refresh()
{
	// DisplayedRootNodes.Reset();
	// for (auto& Item : ChildNodes)
	// {
	// 	if (Item->IsVisible())
	// 	{
	// 		DisplayedRootNodes.Add(Item);
	// 	}
	// }
	// SetTreeItemsSource(&DisplayedRootNodes);
}


void SActImageTreeView::OnHitBoxesChanged(UActAnimation* InActAnimation)
{
	TArray<FActActionHitBoxData>& InHitBoxData = InActAnimation->ActActionHitBoxes;
	FName HitBoxesFolderName("HitBoxesFolder");
	auto FindElement = DisplayedRootNodes.FindByPredicate([HitBoxesFolderName](TSharedRef<SActImageTreeViewTableRow> InNode)
	{
		return InNode->GetNodeName() == HitBoxesFolderName;
	});
	TSharedPtr<SActImageTreeViewTableRow> HitBoxesFolder;
	if (!FindElement)
	{
		HitBoxesFolder = SNew(SActImageTreeViewTableRow, SharedThis(this), HitBoxesFolderName, ENovaTreeViewTableRowType::Folder);
		DisplayedRootNodes.Add(HitBoxesFolder.ToSharedRef());
	}
	else
	{
		HitBoxesFolder = *FindElement;
	}
	int32 HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	{
		FName HitBoxName("HitBox");
		for (int32 count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
		{
			// ** TODO: fix
			TSharedRef<SActImageTreeViewTableRow> NewTreeViewNode = SNew(SActImageTreeViewTableRow, SharedThis(this), HitBoxName, ENovaTreeViewTableRowType::Notifies);
			NewTreeViewNode->SetParent(HitBoxesFolder);
			// DisplayedRootNodes.Add(NewTreeViewNode);
		}
	}
	int32 Index = 0;
	for (FActActionHitBoxData& InHitBox : InHitBoxData)
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetContentAsHitBox(InHitBox);
	}
	int32 ChildCount = HitBoxesFolder->GetChildNodes().Num();
	while (Index < ChildCount)
	{
		HitBoxesFolder->GetChildByIndex(Index++)->RemoveFromParent();
	}
	// SetTreeItemsSource(&DisplayedRootNodes);
	RequestListRefresh();
}


TSharedRef<ITableRow> SActImageTreeView::OnGenerateRow(TSharedRef<SActImageTreeViewTableRow> InTreeViewTableRow, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedPtr<SActImageTrackLaneWidget> TrackLane = TreeViewTableRow2TrackLaneWidget.FindRef(InTreeViewTableRow).Pin();
	if (!TrackLane.IsValid())
	{
		// Add a track slot for the row
		TrackLane = ActImageTrackAreaPanel->MakeTrackLane(InTreeViewTableRow);
		TreeViewTableRow2TrackLaneWidget.Add(InTreeViewTableRow, TrackLane);
	}
	return InTreeViewTableRow;
}
