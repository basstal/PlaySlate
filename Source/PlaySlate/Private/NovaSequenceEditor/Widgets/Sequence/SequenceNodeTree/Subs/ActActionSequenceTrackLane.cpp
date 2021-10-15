#include "ActActionSequenceTrackLane.h"


void SActActionSequenceTrackLane::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InDisplayNode, const TSharedRef<SActActionSequenceTreeView>& InTreeView)
{
	DisplayNode = InDisplayNode;
	TreeView = InTreeView;

	TSharedRef<SWidget> Widget = InArgs._Content.Widget;

	// if (DisplayNode->IsResizable())
	// {
	// 	Widget = SNew(SOverlay)
	// 		+ SOverlay::Slot()
	// 		[
	// 			InArgs._Content.Widget
	// 		]
	//
	// 		+ SOverlay::Slot()
	// 		.VAlign(VAlign_Bottom)
	// 		[
	// 			SNew(SResizeArea, DisplayNode)
	// 		];
	// }

	SetVisibility(EVisibility::SelfHitTestInvisible);

	ChildSlot
		.HAlign(HAlign_Fill)
		.Padding(0)
		[
			Widget
		];
}
