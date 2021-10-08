#include "ActActionSequenceTreeView.h"

#include "Subs/ActActionSequenceTrackLane.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTrackArea.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceDisplayNode.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceNodeTree.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"


void SActActionSequenceTreeViewRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const TSharedRef<FActActionSequenceDisplayNode>& InNode)
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

TOptional<EItemDropZone> SActActionSequenceTreeViewRow::OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceDisplayNode> DisplayNode)
{
	return TOptional<EItemDropZone>();
}

FReply SActActionSequenceTreeViewRow::OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceDisplayNode> DisplayNode)
{
	return FReply::Unhandled();
}

FMargin SActActionSequenceTreeViewRow::GetRowPadding() const
{
	TSharedPtr<FActActionSequenceDisplayNode> PinnedNode = Node.Pin();
	TSharedPtr<FActActionSequenceDisplayNode> ParentNode = PinnedNode ? PinnedNode->GetParentOrRoot() : nullptr;

	if (ParentNode.IsValid() && ParentNode->GetType() == ActActionSequence::ESequenceNodeType::Root && ParentNode->GetChildNodes()[0] != PinnedNode)
	{
		return FMargin(0.f, 1.f, 0.f, 0.f);
	}
	return FMargin(0.f, 0.f, 0.f, 0.f);
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

void SActActionSequenceTreeViewRow::AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& Lane)
{
	TrackLaneReference = Lane;
}

void SActActionSequenceTreeView::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceNodeTree>& InNodeTree, TSharedRef<SActActionSequenceTrackArea> InTrackArea)
{
	SequenceNodeTree = InNodeTree;
	TrackArea = InTrackArea;

	HeaderRow = SNew(SHeaderRow).Visibility(EVisibility::Collapsed);
	OnGetContextMenuContent = InArgs._OnGetContextMenuContent;

	SetupColumns(InArgs);

	STreeView::Construct(
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

void SActActionSequenceTreeView::SetupColumns(const FArguments& InArgs)
{
	auto GenerateOutlinerLambda = [](const TSharedRef<FActActionSequenceDisplayNode>& InDisplayNode, const TSharedRef<SActActionSequenceTreeViewRow>& InTreeViewRow)
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
	STreeView<TSharedRef<FActActionSequenceDisplayNode, ESPMode::Fast>>::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
}

int32 SActActionSequenceTreeView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	LayerId = STreeView<TSharedRef<FActActionSequenceDisplayNode, ESPMode::Fast>>::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
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

TSharedRef<ITableRow> SActActionSequenceTreeView::OnGenerateRow(TSharedRef<FActActionSequenceDisplayNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SActActionSequenceTreeViewRow> Row = SNew(SActActionSequenceTreeViewRow, OwnerTable, InDisplayNode).OnGenerateWidgetForColumn(this, &SActActionSequenceTreeView::GenerateWidgetFromColumn);

	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedPtr<FActActionSequenceDisplayNode> SectionAuthority = InDisplayNode->GetSectionAreaAuthority();
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


void SActActionSequenceTreeView::AddRootNodes(TSharedPtr<FActActionSequenceDisplayNode> SequenceDisplayNode)
{
	RootNodes.Add(SequenceDisplayNode.ToSharedRef());
}

void SActActionSequenceTreeView::Refresh()
{
	RootNodes.Reset();

	for (auto& Item : SequenceNodeTree->GetRootNodes())
	{
		if (Item->IsVisible() /*&& Item->IsPinned() == bShowPinnedNodes*/)
		{
			RootNodes.Add(Item);
		}
	}
}
