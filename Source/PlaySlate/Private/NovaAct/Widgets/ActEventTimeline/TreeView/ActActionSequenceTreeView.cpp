#include "ActActionSequenceTreeView.h"

#include "PlaySlate.h"
#include "NovaAct/Widgets/ActEventTimeline/TreeView/ActActionSequenceTrackArea.h"
#include "NovaAct/Widgets/ActEventTimeline/TreeView/Subs/ActActionSequenceTrackLane.h"
#include "NovaAct/ActEventTimeline/TreeView/ActActionSequenceTreeViewNode.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewHorizontalBox.h"
// #include "NovaAct/Widgets/ActEventTimeline/TreeView/ActActionOutlinerTreeNode.h"
#include "Subs/ActActionSequenceTreeViewRow.h"


void SActActionSequenceTreeView::Construct(const FArguments& InArgs, const TSharedRef<SActActionSequenceTrackArea>& InTrackArea)
{
	TrackArea = InTrackArea;

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed);
	HeaderRow->AddColumn(SHeaderRow::Column(FName("Outliner")).FillWidth(1.0f));

	STreeView::Construct(
		STreeView::FArguments()
		.TreeItemsSource(&DisplayedRootNodes)
		.SelectionMode(ESelectionMode::Single)
		.OnGenerateRow(this, &SActActionSequenceTreeView::OnGenerateRow)
		.OnGetChildren(this, &SActActionSequenceTreeView::OnGetChildren)
		.OnExpansionChanged(this, &SActActionSequenceTreeView::OnExpansionChanged)
		.HeaderRow(HeaderRow)
		.ExternalScrollbar(InArgs._ExternalScrollbar)
		.HighlightParentNodesForSelection(true)
		.AllowInvisibleItemSelection(true)
	);
}

TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode)
		.OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);

	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	const TSharedPtr<FActActionSequenceTreeViewNode> SectionAuthority = InDisplayNode->GetSectionAreaAuthority();
	if (SectionAuthority.IsValid())
	{
		TSharedPtr<SActActionSequenceTrackLane> TrackLane = TrackArea->FindTrackSlot(SectionAuthority.ToSharedRef());
		if (!TrackLane.IsValid())
		{
			// Add a track slot for the row
			SectionAuthority->MakeWidgetForTrackArea();
			TrackLane = TrackArea->AddTrackSlot(SectionAuthority.ToSharedRef());
		}

		if (ensure(TrackLane.IsValid()))
		{
			Row->AddTrackAreaReference(TrackLane.ToSharedRef());
		}
	}
	return Row;
}

TSharedRef<SWidget> SActActionSequenceTreeView::GenerateWidgetFromColumn(const TSharedRef<FActActionSequenceTreeViewNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const
{
	if (ColumnId == "Outliner")
	{
		return InNode->MakeOutlinerWidget(Row);
	}
	return SNullWidget::NullWidget;
}

void SActActionSequenceTreeView::OnGetChildren(TSharedRef<FActActionSequenceTreeViewNode> InParent, TArray<TSharedRef<FActActionSequenceTreeViewNode>>& OutChildren) const
{
	for (const auto& Node : InParent->GetChildNodes())
	{
		if (!Node->IsHidden())
		{
			OutChildren.Add(Node);
		}
	}
}

void SActActionSequenceTreeView::OnExpansionChanged(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, bool bIsExpanded)
{
	UE_LOG(LogNovaAct, Log, TEXT("InDisplayNode->GetPathName : %s, bIsExpanded : %d"), *InDisplayNode->GetPathName(), bIsExpanded);
	for (const TSharedRef<FActActionSequenceTreeViewNode>& ChildNode : InDisplayNode->GetChildNodes())
	{
		ChildNode->SetVisible(bIsExpanded ? EVisibility::Visible : EVisibility::Collapsed);
	}
}


void SActActionSequenceTreeView::Refresh()
{
	DisplayedRootNodes.Reset();
	for (auto& Item : ChildNodes)
	{
		if (Item.Value->IsVisible())
		{
			DisplayedRootNodes.Add(Item.Value);
		}
	}
	SetTreeItemsSource(&DisplayedRootNodes);
}


void SActActionSequenceTreeView::OnHitBoxesChanged(UActAnimation* InActAnimation)
{
	TArray<FActActionHitBoxData>& InHitBoxData = InActAnimation->ActActionHitBoxes;
	const TSharedRef<FActActionSequenceTreeViewNode> HitBoxesFolder = FindOrCreateFolder(FName("HitBoxesFolder"));
	int HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	{
		for (int count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
		{
			TSharedRef<FActActionSequenceTreeViewNode> NewTreeViewNode = MakeShareable(new FActActionSequenceTreeViewNode("HitBox", ENovaSequenceNodeType::State));
			NewTreeViewNode->SetParent(HitBoxesFolder);
		}
	}
	int Index = 0;
	for (FActActionHitBoxData& InHitBox : InHitBoxData)
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetContentAsHitBox(InHitBox);
	}
	while (Index < HitBoxesFolder->GetChildNodes().Num())
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetVisible(EVisibility::Collapsed);
	}
	Refresh();
}


TSharedRef<FActActionSequenceTreeViewNode> SActActionSequenceTreeView::FindOrCreateFolder(const FName& InName)
{
	TSharedRef<FActActionSequenceTreeViewNode>* FindNode = ChildNodes.Find(InName);
	if (!FindNode)
	{
		TSharedRef<FActActionSequenceTreeViewNode> Folder = MakeShareable(new FActActionSequenceTreeViewNode(InName, ENovaSequenceNodeType::Folder));
		return Folder;
	}
	return *FindNode;
}
