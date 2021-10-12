#include "ActActionSequenceTreeView.h"

#include "Subs/ActActionSequenceTrackLane.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceNodeTree.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"


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
	TSharedPtr<FActActionSequenceTreeViewNode> ParentNode = PinnedNode ? PinnedNode->GetParentOrRoot() : nullptr;

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

void SActActionSequenceTreeViewRow::AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& Lane)
{
	TrackLaneReference = Lane;
}

void SActActionSequenceTreeView::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InNodeTree, TSharedRef<SActActionSequenceTrackArea> InTrackArea)
{
	SequenceNodeTree = InNodeTree;
	TrackArea = InTrackArea;

	HeaderRow = SNew(SHeaderRow).Visibility(EVisibility::Collapsed);
	OnGetContextMenuContent = InArgs._OnGetContextMenuContent;

	SetupColumns(InArgs);

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

void SActActionSequenceTreeView::SetupColumns(const FArguments& InArgs)
{
	auto GenerateOutlinerLambda = [](const TSharedRef<FActActionSequenceTreeViewNode>& InDisplayNode, const TSharedRef<SActActionSequenceTreeViewRow>& InTreeViewRow)
	{
		return InDisplayNode->GenerateContainerWidgetForOutliner(InTreeViewRow);
	};
	Columns.Add("Outliner", FActActionSequenceTreeViewColumn(GenerateOutlinerLambda, 1.0f));

	// Now populate the header row with the columns
	for (TTuple<FName, FActActionSequenceTreeViewColumn>& Pair : Columns)
	{
		if (Pair.Key != FName("TrackArea"))
		{
			HeaderRow->AddColumn(
				SHeaderRow::Column(Pair.Key)
				.FillWidth(Pair.Value.Width)
			);
		}
	}
}

void SActActionSequenceTreeView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	STreeView<TSharedRef<FActActionSequenceTreeViewNode, ESPMode::Fast>>::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

int32 SActActionSequenceTreeView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = STreeView<TSharedRef<FActActionSequenceTreeViewNode, ESPMode::Fast>>::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	// These are updated in both tick and paint since both calls can cause changes to the cached rows and the data needs
	// to be kept synchronized so that external measuring calls get correct and reliable results.
	// if (bPhysicalNodesNeedUpdate)
	// {
	// 	PhysicalNodes.Reset();
	// 	CachedRowGeometry.GenerateValueArray(PhysicalNodes);
	//
	// 	PhysicalNodes.Sort([](const FCachedGeometry& A, const FCachedGeometry& B) {
	// 		return A.PhysicalTop < B.PhysicalTop;
	// 	});
	// }

	// if (HighlightRegion.IsSet())
	// {
	// 	// Black tint for highlighted regions
	// 	FSlateDrawElement::MakeBox(
	// 		OutDrawElements,
	// 		LayerId+1,
	// 		AllottedGeometry.ToPaintGeometry(FVector2D(2.f, HighlightRegion->Top - 4.f), FVector2D(AllottedGeometry.Size.X - 4.f, 4.f)),
	// 		FEditorStyle::GetBrush("Sequencer.TrackHoverHighlight_Top"),
	// 		ESlateDrawEffect::None,
	// 		FLinearColor::Black
	// 	);
	// 	
	// 	FSlateDrawElement::MakeBox(
	// 		OutDrawElements,
	// 		LayerId+1,
	// 		AllottedGeometry.ToPaintGeometry(FVector2D(2.f, HighlightRegion->Bottom), FVector2D(AllottedGeometry.Size.X - 4.f, 4.f)),
	// 		FEditorStyle::GetBrush("Sequencer.TrackHoverHighlight_Bottom"),
	// 		ESlateDrawEffect::None,
	// 		FLinearColor::Black
	// 	);
	// }

	return LayerId + 1;
}

TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode).OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);

	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedPtr<FActActionSequenceTreeViewNode> SectionAuthority = InDisplayNode->GetSectionAreaAuthority();
	if (SectionAuthority.IsValid())
	{
		TSharedPtr<SActActionSequenceTrackLane> TrackLane = TrackArea->FindTrackSlot(SectionAuthority.ToSharedRef());

		if (!TrackLane.IsValid())
		{
			// Add a track slot for the row
			TAttribute<TRange<double>> ViewRange = ActActionSequence::FActActionAnimatedRange::WrapAttribute(TAttribute<ActActionSequence::FActActionAnimatedRange>::Create(TAttribute<ActActionSequence::FActActionAnimatedRange>::FGetter::CreateSP(SequenceNodeTree->GetSequence(), &FActActionSequenceController::GetViewRange)));

			TrackLane = SNew(SActActionSequenceTrackLane, SectionAuthority.ToSharedRef(), SharedThis(this))
				//.IsEnabled(!InDisplayNode->GetSequencer().IsReadOnly())
				[
					SectionAuthority->GenerateWidgetForSectionArea(ViewRange)
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
	const FActActionSequenceTreeViewColumn* TreeViewColumn = Columns.Find(ColumnId);
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

// bool SActActionSequenceTreeView::IsNodeFiltered(const TSharedRef<FActActionSequenceTreeViewNode> Node) const
// {
// 	return FilteredNodes.Contains(Node);
// }


void SActActionSequenceTreeView::AddDisplayNode(TSharedPtr<FActActionSequenceTreeViewNode> SequenceDisplayNode)
{
	DisplayedRootNodes.Add(SequenceDisplayNode.ToSharedRef());
	SetTreeItemsSource(GetDisplayedRootNodes());
}

void SActActionSequenceTreeView::Refresh()
{
	DisplayedRootNodes.Reset();

	for (auto& Item : SequenceNodeTree->GetChildNodes())
	{
		if (Item->IsVisible() /*&& Item->IsPinned() == bShowPinnedNodes*/)
		{
			DisplayedRootNodes.Add(Item);
		}
	}
}
