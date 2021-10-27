// #pragma once
//
// class SActActionSequenceTreeViewRow;
//
// class SActImageTreeViewTableRow;
//
// class SActActionOutlinerTreeNode : public SMultiColumnTableRow<TSharedRef<SActImageTreeViewTableRow>>
// {
// public:
// 	SLATE_BEGIN_ARGS(SActActionOutlinerTreeNode) { }
//
// 		SLATE_ATTRIBUTE(const FSlateBrush*, IconBrush)
// 		SLATE_ATTRIBUTE(const FSlateBrush*, IconOverlayBrush)
// 		SLATE_ATTRIBUTE(FSlateColor, IconColor)
// 		SLATE_ATTRIBUTE(FText, IconToolTipText)
// 		SLATE_NAMED_SLOT(FArguments, CustomContent)
//
// 	SLATE_END_ARGS()
//
// 	SActActionOutlinerTreeNode();
//
// 	virtual ~SActActionOutlinerTreeNode() override;
//
// 	void Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& Node, const TSharedRef<SActActionSequenceTreeViewRow>& InTableRow);
//
// 	/**
// 	* @return The parent of this node. Will return null if this node is part of the FSequencerNodeTree::GetRootNodes array.
// 	*/
// 	TSharedPtr<SActImageTreeViewTableRow> GetParent() const;
//
//
// 	/** Callback for checking whether the node label can be edited. */
// 	bool IsNodeLabelReadOnly() const;
//
// 	FSlateFontInfo GetDisplayNameFont() const;
//
// 	FSlateColor GetDisplayNameColor() const;
//
// 	bool VerifyNodeTextChanged(const FText& NewLabel, FText& OutErrorMessage) const;
//
// 	void HandleNodeLabelTextCommitted(const FText& NewLabel, ETextCommit::Type CommitType) const;
//
// 	FSlateColor GetNodeBackgroundTint() const;
//
// protected:
// 	/** Holds the editable text label widget. */
// 	TSharedPtr<SInlineEditableTextBlock> EditableLabel;
//
// 	TSharedPtr<SActImageTreeViewTableRow> DisplayNode;
//
// 	/** The parent of this node*/
// 	TWeakPtr<SActImageTreeViewTableRow> ParentNode;
//
// 	/** True if this node is a top level node, at the root of the tree, false otherwise */
// 	bool bIsOuterTopLevelNode;
//
// 	/** True if this is a top level node inside or a folder, otherwise false. */
// 	bool bIsInnerTopLevelNode;
//
// 	/** The table row style used for nodes in the tree. This is required as we don't actually use the tree for selection. */
// 	const FTableRowStyle* TableRowStyle;
// };
