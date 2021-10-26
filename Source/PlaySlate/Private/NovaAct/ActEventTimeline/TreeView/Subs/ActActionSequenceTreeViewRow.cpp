#include "ActActionSequenceTreeViewRow.h"

#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewNode.h"


void SActActionSequenceTreeViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<SActTreeViewNode>& InNode)
{
	Node = InNode;
	OnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;
	SMultiColumnTableRow::Construct(
		SMultiColumnTableRow::FArguments()
		.OnDragDetected(this, &SActActionSequenceTreeViewRow::OnDragDetected)
		.OnCanAcceptDrop(this, &SActActionSequenceTreeViewRow::OnCanAcceptDrop)
		.OnAcceptDrop(this, &SActActionSequenceTreeViewRow::OnAcceptDrop)
		.ShowSelection(InNode->IsSelectable())
		.Padding(this, &SActActionSequenceTreeViewRow::GetRowPadding),
		OwnerTableView);
}

TOptional<EItemDropZone> SActActionSequenceTreeViewRow::OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<SActTreeViewNode> DisplayNode)
{
	return TOptional<EItemDropZone>();
}

FReply SActActionSequenceTreeViewRow::OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<SActTreeViewNode> DisplayNode)
{
	return FReply::Unhandled();
}

FMargin SActActionSequenceTreeViewRow::GetRowPadding() const
{
	const TSharedPtr<SActTreeViewNode> PinnedNode = Node.Pin();
	const TSharedPtr<SActTreeViewNode> ParentNode = PinnedNode ? PinnedNode->GetParentNode() : nullptr;

	const TArray<TSharedRef<SActTreeViewNode>>& ChildNodes = ParentNode->GetChildNodes();
	if (ParentNode.IsValid() && ParentNode->GetType() == ENovaSequenceNodeType::Root && ChildNodes.Num() > 0 && ChildNodes[0] != PinnedNode)
	{
		return FMargin(0.0f, 1.f, 0.0f, 0.0f);
	}
	return FMargin(0.0f, 0.0f, 0.0f, 0.0f);
}

TSharedRef<SWidget> SActActionSequenceTreeViewRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<SWidget> ColumnWidget = SNullWidget::NullWidget;
	if (OnGenerateWidgetForColumn.IsBound())
	{
		ColumnWidget = OnGenerateWidgetForColumn.Execute(Node.Pin().ToSharedRef(), InColumnName, SharedThis(this));
	}

	return SNew(SOverlay)
		+ SOverlay::Slot()
		[
			ColumnWidget.ToSharedRef()
		];
}

// void SActActionSequenceTreeViewRow::AddTrackAreaReference(const TSharedRef<SActTreeViewTrackCarWidget>& InTrackLane)
// {
// 	TargetTrackLane = InTrackLane;
// }
