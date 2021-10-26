#include "ActTreeViewHorizontalBox.h"

#include "Common/NovaConst.h"
#include "NovaAct/NovaActEditor.h"

#include "NovaAct/ActEventTimeline/TreeView/ActTreeView.h"
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewNode.h"
#include "NovaAct/ActEventTimeline/TreeView/ActTreeViewTrackAreaPanel.h"
#include "NovaAct/ActEventTimeline/TreeView/Subs/ActActionSequenceTreeViewRow.h"

#include "Widgets/Layout/SScrollBorder.h"

#define LOCTEXT_NAMESPACE "NovaAct"

void SActTreeViewHorizontalBox::Construct(const FArguments& InArgs)
{
	const TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar)
		.Thickness(FVector2D(9.0f, 9.0f));
	const TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar)
		.Thickness(FVector2D(9.0f, 9.0f));
	ActTreeViewTrackAreaPanel = SNew(SActTreeViewTrackAreaPanel);
	TSharedRef<SActTreeView> TreeView = SNew(SActTreeView, ActTreeViewTrackAreaPanel.ToSharedRef())
		.ExternalScrollbar(ScrollBar)
		.OnGenerateRow(this, &SActTreeViewHorizontalBox::OnGenerateRow);
	TSharedRef<SActTreeViewTrackAreaPanel> TrackAreaPinned = SNew(SActTreeViewTrackAreaPanel);
	TSharedRef<SActTreeView> TreeViewPinned = SNew(SActTreeView, TrackAreaPinned)
		.ExternalScrollbar(PinnedAreaScrollBar)
		.OnGenerateRow(this, &SActTreeViewHorizontalBox::OnGenerateRow);


	auto FillLeftAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return DB->GetData();
	}));
	auto FillRightAttr = TAttribute<float>::Create(TAttribute<float>::FGetter::CreateLambda([]()
	{
		auto DB = GetDataBinding(float, "ColumnFillCoefficientsLeft");
		return 1 - DB->GetData();
	}));

	Slot()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SScrollBorder, TreeView)
			[
				SNew(SHorizontalBox)

				// outliner tree
				+ SHorizontalBox::Slot()
				.FillWidth(FillLeftAttr)
				[
					SNew(SBox)
					[
						TreeView
					]
				]

				// track area
				+ SHorizontalBox::Slot()
				.FillWidth(FillRightAttr)
				[
					SNew(SBox)
				.Padding(NovaConst::ResizeBarPadding)
				.Clipping(EWidgetClipping::ClipToBounds)
					[
						ActTreeViewTrackAreaPanel.ToSharedRef()
					]
				]
			]
		]

		+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		[
			ScrollBar
		]
	];
}


TSharedRef<ITableRow> SActTreeViewHorizontalBox::OnGenerateRow(TSharedRef<SActTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	TSharedRef<SActTreeViewNode> Row = SNew(SActTreeViewNode, OwnerTable);

	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	const TSharedPtr<SActTreeViewNode> SectionAuthority = InDisplayNode->GetSectionAreaAuthority();
	if (SectionAuthority.IsValid())
	{
		TSharedPtr<SActTreeViewTrackLaneWidget> TrackLane = TreeViewNode2TrackLane.FindRef(SectionAuthority.ToSharedRef()).Pin();
		if (!TrackLane.IsValid())
		{
			// Add a track slot for the row
			// SectionAuthority->MakeWidgetForTrackArea();
			TrackLane = ActTreeViewTrackAreaPanel->MakeTrackLaneWithTreeViewNode(SectionAuthority.ToSharedRef());
			TreeViewNode2TrackLane.Add(SectionAuthority, TrackLane);
		}

		// if (ensure(TrackLane.IsValid()))
		// {
		// 	Row->AddTrackAreaReference(TrackLane.ToSharedRef());
		// }
	}
	return Row;
}

#undef LOCTEXT_NAMESPACE
