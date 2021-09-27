#pragma once
#include "Editor/ActActionSequenceDisplayNode.h"

class SActActionOutlinerTreeNode : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionOutlinerTreeNode)
		{
		}
		// SLATE_ATTRIBUTE(const FSlateBrush*, IconBrush)
		// SLATE_ATTRIBUTE(const FSlateBrush*, IconOverlayBrush)
		// SLATE_ATTRIBUTE(FSlateColor, IconColor)
		// SLATE_ATTRIBUTE(FText, IconToolTipText)
		// SLATE_NAMED_SLOT(FArguments, CustomContent)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, TSharedRef<FActActionSequenceDisplayNode> Node);

	/**
	* @return The parent of this node. Will return null if this node is part of the FSequencerNodeTree::GetRootNodes array.
	*/
	TSharedPtr<FActActionSequenceDisplayNode> GetParent() const
	{
		TSharedPtr<FActActionSequenceDisplayNode> Pinned = ParentNode.Pin();
		return (Pinned && Pinned->GetType() != ActActionSequence::ESequenceNodeType::Root) ? Pinned : nullptr;
	}
protected:
	TSharedPtr<FActActionSequenceDisplayNode> DisplayNode;
	/** The parent of this node*/
	TWeakPtr<FActActionSequenceDisplayNode> ParentNode;
	/** True if this node is a top level node, at the root of the tree, false otherwise */
	bool bIsOuterTopLevelNode;

	/** True if this is a top level node inside or a folder, otherwise false. */
	bool bIsInnerTopLevelNode;
	/** The table row style used for nodes in the tree. This is required as we don't actually use the tree for selection. */
	const FTableRowStyle* TableRowStyle;
};
