// #include "ActActionSequenceTreeView.h"
//
// #include "ActActionSequenceTreeViewRow.h"
// #include "Editor/ActActionSequenceDisplayNode.h"
//
//
// void SActActionSequenceTreeView::Construct(const FArguments& InArgs, TSharedPtr<FActActionSequenceNodeTree> InNodeTree, TSharedPtr<SActActionTrackArea> InTrackArea)
// {
// 	ActActionSequenceNodeTree = InNodeTree;
// 	ActActionTrackAreaWidget = InTrackArea;
//
// 	HeaderRow = SNew(SHeaderRow).Visibility(EVisibility::Collapsed);
// 	OnGetContextMenuContent = InArgs._OnGetContextMenuContent;
//
// 	STreeView::Construct(
// 		STreeView::FArguments()
// 		.TreeItemsSource(&RootNodes)
// 		.SelectionMode(ESelectionMode::Multi)
// 		.OnGenerateRow(this, &SActActionSequenceTreeView::OnGenerateRow)
// 		.OnGetChildren(this, &SActActionSequenceTreeView::OnGetChildren)
// 		.HeaderRow(HeaderRow)
// 		// .
//
// 	);
// }
//
//
// TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceDisplayNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
// {
// 	// ** TODO:填充显示信息
// 	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode).OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);
// 	return Row;
// }
//
// TSharedRef<SWidget> SActActionSequenceTreeView::GenerateWidgetFromColumn(const TSharedRef<FActActionSequenceDisplayNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const
// {
// 	const FActActionSequenceTreeViewColumn* TreeViewColumn = Columns.Find(ColumnId);
// 	if (TreeViewColumn)
// 	{
// 		return TreeViewColumn->Generator(InNode, Row);
// 	}
// 	return SNullWidget::NullWidget;
// }
//
// void SActActionSequenceTreeView::OnGetChildren(TSharedRef<FActActionSequenceDisplayNode> InParent, TArray<TSharedRef<FActActionSequenceDisplayNode>>& OutChildren) const
// {
// 	for (const auto& Node : InParent->GetChildNodes())
// 	{
// 		if (!Node->IsHidden())
// 		{
// 			OutChildren.Add(Node);
// 		}
// 	}
// }
//
// bool SActActionSequenceTreeView::IsNodeFiltered(const TSharedRef<FActActionSequenceDisplayNode> Node) const
// {
// 	return FilteredNodes.Contains(Node);
// }
