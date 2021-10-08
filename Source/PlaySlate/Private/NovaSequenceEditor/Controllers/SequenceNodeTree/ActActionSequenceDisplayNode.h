#pragma once

#include "Utils/ActActionSequenceUtil.h"

class FActActionSequenceSectionBase;
class FActActionSequenceNodeTree;
class SActActionSequenceTreeViewRow;
/**
 * 基础的Sequence Node
 */
class FActActionSequenceDisplayNode : public TSharedFromThis<FActActionSequenceDisplayNode>
{
public:
	/**
	* Create and initialize a new instance.
	* 
	* @param InParentTree The tree this node is in.
	*/
	FActActionSequenceDisplayNode(const TSharedRef<FActActionSequenceNodeTree>& InParentTree, FName InNodeName = NAME_None);

	virtual ~FActActionSequenceDisplayNode();


protected:
	/** FIX:List of children belonging to this node */
	TArray<TSharedRef<FActActionSequenceDisplayNode>> ChildNodes;
	/** FIX:Parent tree that this node is in */
	// FActActionSequenceNodeTree& ParentTree;
	/** FIX:The name identifier of this node */
	FName NodeName;
	/** The parent of this node*/
	TWeakPtr<FActActionSequenceDisplayNode> ParentNode;
	/** All of the sequencer sections in this node */
	TArray<TSharedRef<FActActionSequenceSectionBase>> Sections;
	/** Parent tree that this node is in */
	TSharedPtr<FActActionSequenceNodeTree> ParentTree;
public:
	/**
	* @return FIX:A List of all Child nodes belonging to this node
	*/
	const TArray<TSharedRef<FActActionSequenceDisplayNode>>& GetChildNodes() const;

	FString GetPathName() const;
	/**
	* @return 是否被隐藏
	*/
	bool IsHidden() const;
	TSharedRef<SWidget> GenerateContainerWidgetForOutliner(const TSharedRef<SActActionSequenceTreeViewRow>& InRow);

	ActActionSequence::ESequenceNodeType GetType();
	/** @return Whether or not this node can be selected */
	bool IsSelectable() const;

	
	void SetParent(TSharedPtr<FActActionSequenceDisplayNode> InParent, int32 DesiredChildIndex);

	/**
	* @return The parent of this node. Will return null if this node is part of the FSequencerNodeTree::GetRootNodes array.
	*/
	TSharedPtr<FActActionSequenceDisplayNode> GetParent() const;

	/**
	* Get the display node that is ultimately responsible for constructing a section area widget for this node.
	* Could return this node itself, or a parent node
	*/
	TSharedPtr<FActActionSequenceDisplayNode> GetSectionAreaAuthority() const;
	/**
	* @return Whether this node should be displayed on the tree view
	*/
	bool IsVisible() const;
	/**
	* Generates a widget for display in the section area portion of the track area
	* 
	* @param ViewRange	The range of time in the sequencer that we are displaying
	* @return Generated outliner widget
	*/
	TSharedRef<SWidget> GenerateWidgetForSectionArea(const TAttribute<TRange<double>>& ViewRange);

	TArray<TSharedRef<FActActionSequenceSectionBase>>& GetSections();
	float GetNodeHeight() const;

	TSharedRef<FActActionSequenceController> GetSequence() const;
	FFrameRate GetTickResolution() const;

	/**
	* @return The parent of this node, or the symbolic root node if this node is part of the FSequencerNodeTree::GetRootNodes array.
	*/
	TSharedPtr<FActActionSequenceDisplayNode> GetParentOrRoot() const;
	bool CanRenameNode() const;
	/**
	*@return The font used to draw the display name.
	*/
	FSlateFontInfo GetDisplayNameFont() const;
	FSlateColor GetDisplayNameColor() const;
	bool ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const;
	/**
	* Set the node's display name.
	*
	* @param NewDisplayName the display name to set.
	*/
	void SetDisplayName(const FText& NewDisplayName);
};
