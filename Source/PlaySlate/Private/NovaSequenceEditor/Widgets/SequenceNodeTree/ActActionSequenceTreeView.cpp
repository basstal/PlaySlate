#include "ActActionSequenceTreeView.h"

#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/Subs/ActActionSequenceTrackLane.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "Subs/ActActionSequenceTreeViewRow.h"


void SActActionSequenceTreeView::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InActActionSequenceTreeViewNode, TSharedRef<SActActionSequenceTrackArea> InTrackArea)
{
	SequenceTreeViewNode = InActActionSequenceTreeViewNode;
	TrackArea = InTrackArea;

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed);

	// Generate Columns
	auto GenerateOutlinerLambda = [](const TSharedRef<FActActionSequenceTreeViewNode>& InActActionSequenceTreeViewNode, const TSharedRef<SActActionSequenceTreeViewRow>& InTreeViewRow)-> TSharedRef<SActActionOutlinerTreeNode>
	{
		InActActionSequenceTreeViewNode->MakeContainerWidgetForOutliner(InTreeViewRow);
		return InActActionSequenceTreeViewNode->GetActActionOutlinerTreeNode();
	};
	Columns.Add("Outliner", ActActionSequence::FActActionSequenceTreeViewColumn(GenerateOutlinerLambda, 1.0f));
	// Now populate the header row with the columns
	for (TTuple<FName, ActActionSequence::FActActionSequenceTreeViewColumn>& Pair : Columns)
	{
		if (Pair.Key != FName("TrackArea"))
		{
			HeaderRow->AddColumn(
				SHeaderRow::Column(Pair.Key)
				.FillWidth(Pair.Value.Width)
			);
		}
	}

	STreeView::Construct(
		STreeView::FArguments()
		.TreeItemsSource(&DisplayedRootNodes)
		.SelectionMode(ESelectionMode::Multi)
		.OnGenerateRow(this, &SActActionSequenceTreeView::OnGenerateRow)
		.OnGetChildren(this, &SActActionSequenceTreeView::OnGetChildren)
		.HeaderRow(HeaderRow)
		.ExternalScrollbar(InArgs._ExternalScrollbar)
		.HighlightParentNodesForSelection(true)
		.AllowInvisibleItemSelection(true)
	);
}


void SActActionSequenceTreeView::AddDisplayNode(TSharedPtr<FActActionSequenceTreeViewNode> SequenceDisplayNode)
{
	DisplayedRootNodes.Add(SequenceDisplayNode.ToSharedRef());
	SetTreeItemsSource(&DisplayedRootNodes);
}

TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode)
		.OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);

	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedPtr<FActActionSequenceTreeViewNode> SectionAuthority = InDisplayNode->GetSectionAreaAuthority();
	if (SectionAuthority.IsValid())
	{
		TSharedPtr<SActActionSequenceTrackLane> TrackLane = TrackArea->FindTrackSlot(SectionAuthority.ToSharedRef());
		if (!TrackLane.IsValid())
		{
			// Add a track slot for the row
			SectionAuthority->MakeWidgetForSectionArea();
			TrackLane = SNew(SActActionSequenceTrackLane, SectionAuthority.ToSharedRef(), SharedThis(this))
			[
				SectionAuthority->GetActActionSectionWidget()
			];
			TrackArea->AddTrackSlot(SectionAuthority.ToSharedRef(), TrackLane.ToSharedRef());
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
	const ActActionSequence::FActActionSequenceTreeViewColumn* TreeViewColumn = Columns.Find(ColumnId);
	if (TreeViewColumn)
	{
		return TreeViewColumn->Generator(InNode, Row);
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

void SActActionSequenceTreeView::Refresh()
{
	DisplayedRootNodes.Reset();
	for (auto& Item : SequenceTreeViewNode.Pin()->GetChildNodes())
	{
		if (Item->IsVisible())
		{
			DisplayedRootNodes.Add(Item);
		}
	}
}
