#include "ActActionSequenceTreeViewRow.h"

#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"


void SActActionSequenceTreeViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<FActActionSequenceTreeViewNode>& InNode)
{
	Node = InNode;
	OnGenerateWidgetForColumn = InArgs._OnGenerateWidgetForColumn;
	bool bIsSelectable = InNode->IsSelectable();

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
	TSharedPtr<FActActionSequenceTreeViewNode> PinnedNode = Node.Pin();
	TSharedPtr<FActActionSequenceTreeViewNode> ParentNode = PinnedNode ? PinnedNode->GetParentNode() : nullptr;

	const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& ChildNodes = ParentNode->GetChildNodes();
	if (ParentNode.IsValid() && ParentNode->GetType() == ActActionSequence::ESequenceNodeType::Root && ChildNodes.Num() > 0 && ChildNodes[0] != PinnedNode)
	{
		return FMargin(0.f, 1.f, 0.f, 0.f);
	}
	return FMargin(0.f, 0.f, 0.f, 0.f);
}

TSharedRef<SWidget> SActActionSequenceTreeViewRow::GenerateWidgetForColumn(const FName& InColumnName)
{
	TSharedPtr<FActActionSequenceTreeViewNode> PinnedNode = Node.Pin();
	if (PinnedNode.IsValid())
	{
		return OnGenerateWidgetForColumn.Execute(PinnedNode.ToSharedRef(), InColumnName, SharedThis(this));
	}

	return SNullWidget::NullWidget;
}

void SActActionSequenceTreeViewRow::AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& InTrackLane)
{
	TrackLaneReference = InTrackLane;
}
