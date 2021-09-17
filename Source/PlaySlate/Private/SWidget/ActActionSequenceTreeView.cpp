#include "ActActionSequenceTreeView.h"

#include "Editor/ActActionSequenceDisplayNode.h"


void SActActionSequenceTreeViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const FActActionSequenceDisplayNodeRef& InNode)
{
}

TSharedRef<SWidget> SActActionSequenceTreeViewRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<FActActionSequenceDisplayNode> PinnedNode = Node.Pin();
	if (PinnedNode.IsValid())
	{
		return OnGenerateWidgetForColumn.Execute(PinnedNode.ToSharedRef(), InColumnName, SharedThis(this));
	}

	return SNullWidget::NullWidget;
}

void SActActionSequenceTreeView::Construct(const FArguments& InArgs)
{
	// ActActionSequenceNodeTree = InNodeTree;
	// ActActionTrackAreaWidget = InTrackArea;

	HeaderRow = SNew(SHeaderRow).Visibility(EVisibility::Collapsed);
	OnGetContextMenuContent = InArgs._OnGetContextMenuContent;

	STreeView::Construct
	(
		STreeView::FArguments()
		.TreeItemsSource(&RootNodes)
		.SelectionMode(ESelectionMode::Multi)
		.OnGenerateRow(this, &SActActionSequenceTreeView::OnGenerateRow)
		.OnGetChildren(this, &SActActionSequenceTreeView::OnGetChildren)
		.HeaderRow(HeaderRow)
		.ExternalScrollbar(InArgs._ExternalScrollbar)
		.HighlightParentNodesForSelection(true)
		.AllowInvisibleItemSelection(true)
	);
}


TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceDisplayNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	// ** TODO:填充显示信息
	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode).OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);
	return Row;
}

TSharedRef<SWidget> SActActionSequenceTreeView::GenerateWidgetFromColumn(const TSharedRef<FActActionSequenceDisplayNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const
{
	const FActActionSequenceTreeViewColumn* TreeViewColumn = Columns.Find(ColumnId);
	if (TreeViewColumn)
	{
		return TreeViewColumn->Generator(InNode, Row);
	}
	return SNullWidget::NullWidget;
}

void SActActionSequenceTreeView::OnGetChildren(TSharedRef<FActActionSequenceDisplayNode> InParent, TArray<TSharedRef<FActActionSequenceDisplayNode>>& OutChildren) const
{
	for (const auto& Node : InParent->GetChildNodes())
	{
		if (!Node->IsHidden())
		{
			OutChildren.Add(Node);
		}
	}
}

// bool SActActionSequenceTreeView::IsNodeFiltered(const TSharedRef<FActActionSequenceDisplayNode> Node) const
// {
// 	return FilteredNodes.Contains(Node);
// }
