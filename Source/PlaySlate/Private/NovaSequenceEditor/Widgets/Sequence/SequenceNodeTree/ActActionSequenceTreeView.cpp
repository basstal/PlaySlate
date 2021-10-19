#include "ActActionSequenceTreeView.h"

#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/Subs/ActActionSequenceTrackLane.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Sequence/SequenceNodeTree/ActActionOutlinerTreeNode.h"
#include "Subs/ActActionSequenceTreeViewRow.h"


void SActActionSequenceTreeView::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InActActionSequenceTreeViewNode, TSharedRef<SActActionSequenceTrackArea> InTrackArea)
{
	SequenceTreeViewNode = InActActionSequenceTreeViewNode;
	TrackArea = InTrackArea;

	HeaderRow = SNew(SHeaderRow)
		.Visibility(EVisibility::Collapsed);
	HeaderRow->AddColumn(SHeaderRow::Column(FName("Outliner")).FillWidth(1.0f));

	STreeView::Construct(
		STreeView::FArguments()
		.TreeItemsSource(&InActActionSequenceTreeViewNode->GetDisplayedRootNodes())
		.SelectionMode(ESelectionMode::Multi)
		.OnGenerateRow(this, &SActActionSequenceTreeView::OnGenerateRow)
		.OnGetChildren(this, &SActActionSequenceTreeView::OnGetChildren)
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
