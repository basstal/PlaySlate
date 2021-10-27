#include "ActImageHorizontalBox.h"

#include "Common/NovaConst.h"
#include "NovaAct/NovaActEditor.h"

#include "NovaAct/ActEventTimeline/TreeView/ActImageTreeView.h"
#include "NovaAct/ActEventTimeline/TreeView/ActImageTreeViewTableRow.h"
#include "NovaAct/ActEventTimeline/TreeView/ActImageTrackAreaPanel.h"
#include "NovaAct/ActEventTimeline/TreeView/Subs/ActActionSequenceTreeViewRow.h"

#include "Widgets/Layout/SScrollBorder.h"

#define LOCTEXT_NAMESPACE "NovaAct"

SActImageHorizontalBox::~SActImageHorizontalBox()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActImageHorizontalBox::~SActImageHorizontalBox"));
}

void SActImageHorizontalBox::Construct(const FArguments& InArgs)
{
	const TSharedRef<SScrollBar> ScrollBar = SNew(SScrollBar)
		.Thickness(FVector2D(9.0f, 9.0f));
	const TSharedRef<SScrollBar> PinnedAreaScrollBar = SNew(SScrollBar)
		.Thickness(FVector2D(9.0f, 9.0f));
	ActImageTrackAreaPanel = SNew(SActImageTrackAreaPanel);
	ActImageTreeView = SNew(SActImageTreeView)
		.ExternalScrollbar(ScrollBar)
		.OnGenerateRow(this, &SActImageHorizontalBox::OnGenerateRow);
	PinnedActImageTrackAreaPanel = SNew(SActImageTrackAreaPanel);
	PinnedActImageTreeView = SNew(SActImageTreeView)
		.ExternalScrollbar(PinnedAreaScrollBar)
		.OnGenerateRow(this, &SActImageHorizontalBox::OnGenerateRow);


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

	AddSlot()
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SScrollBorder, ActImageTreeView.ToSharedRef())
			[
				SNew(SHorizontalBox)

				// outliner tree
				+ SHorizontalBox::Slot()
				.FillWidth(FillLeftAttr)
				[
					SNew(SBox)
					[
						ActImageTreeView.ToSharedRef()
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
						ActImageTrackAreaPanel.ToSharedRef()
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


TSharedRef<ITableRow> SActImageHorizontalBox::OnGenerateRow(TSharedRef<SActImageTreeViewTableRow> InTreeViewNode, const TSharedRef<STableViewBase>& OwnerTable)
{
	// Ensure the track area is kept up to date with the virtualized scroll of the tree view
	TSharedPtr<SActImageTrackLaneWidget> TrackLane = TreeViewNode2TrackLane.FindRef(InTreeViewNode).Pin();
	if (!TrackLane.IsValid())
	{
		// Add a track slot for the row
		TrackLane = ActImageTrackAreaPanel->MakeTrackLaneWithTreeViewNode(InTreeViewNode);
		TreeViewNode2TrackLane.Add(InTreeViewNode, TrackLane);
	}
	return InTreeViewNode;
}

#undef LOCTEXT_NAMESPACE
