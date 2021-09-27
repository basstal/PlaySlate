#include "ActActionOutlinerTreeNode.h"

void SActActionOutlinerTreeNode::Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceDisplayNode> Node)
{
	DisplayNode = Node;
	bIsOuterTopLevelNode = !Node->GetParent().IsValid();
	bIsInnerTopLevelNode = Node->GetType() != ActActionSequence::ESequenceNodeType::Folder && Node->GetParent().IsValid() && Node->GetParent()->GetType() == ActActionSequence::ESequenceNodeType::Folder;

	if (bIsOuterTopLevelNode)
	{
		// ExpandedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
		// CollapsedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Collapsed" );
	}
	else
	{
		// ExpandedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.DefaultBorder" );
		// CollapsedBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.DefaultBorder" );
	}

	// FMargin InnerNodePadding;
	// if ( bIsInnerTopLevelNode )
	// {
	// 	InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );
	// 	InnerNodePadding = FMargin(0.f, 1.f);
	// }
	// else
	// {
	// 	InnerBackgroundBrush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TransparentBorder" );
	// 	InnerNodePadding = FMargin(0.f);
	// }

	TableRowStyle = &FEditorStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");
	
}
