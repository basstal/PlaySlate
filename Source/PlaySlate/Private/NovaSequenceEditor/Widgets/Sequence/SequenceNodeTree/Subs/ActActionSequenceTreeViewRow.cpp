#include "ActActionSequenceTreeViewRow.h"

#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"


void SActActionSequenceTreeViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<FActActionSequenceTreeViewNode>& InNode)
{
	Node = InNode;
	OnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;
	const bool bIsSelectable = InNode->IsSelectable();

	SMultiColumnTableRow::Construct(
		SMultiColumnTableRow::FArguments()
		.OnDragDetected(this, &SActActionSequenceTreeViewRow::OnDragDetected)
		.OnCanAcceptDrop(this, &SActActionSequenceTreeViewRow::OnCanAcceptDrop)
		.OnAcceptDrop(this, &SActActionSequenceTreeViewRow::OnAcceptDrop)
		.ShowSelection(bIsSelectable)
		.Padding(this, &SActActionSequenceTreeViewRow::GetRowPadding),
		OwnerTableView);
}

TOptional<EItemDropZone> SActActionSequenceTreeViewRow::OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceTreeViewNode> DisplayNode)
{
	return TOptional<EItemDropZone>();
}

FReply SActActionSequenceTreeViewRow::OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceTreeViewNode> DisplayNode)
{
	return FReply::Unhandled();
}

FMargin SActActionSequenceTreeViewRow::GetRowPadding() const
{
	const TSharedPtr<FActActionSequenceTreeViewNode> PinnedNode = Node.Pin();
	const TSharedPtr<FActActionSequenceTreeViewNode> ParentNode = PinnedNode ? PinnedNode->GetParentNode() : nullptr;

	const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& ChildNodes = ParentNode->GetChildNodes();
	if (ParentNode.IsValid() && ParentNode->GetType() == ActActionSequence::ESequenceNodeType::Root && ChildNodes.Num() > 0 && ChildNodes[0] != PinnedNode)
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

void SActActionSequenceTreeViewRow::AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& InTrackLane)
{
	TrackLaneReference = InTrackLane;
}
