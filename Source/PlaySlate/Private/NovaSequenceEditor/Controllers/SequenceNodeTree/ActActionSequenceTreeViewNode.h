#pragma once

#include "Utils/ActActionSequenceUtil.h"

class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
/**
 * 基础的Sequence Node
 */
class FActActionSequenceTreeViewNode : public TSharedFromThis<FActActionSequenceTreeViewNode>
{
public:
	/**
	* 构造一个树节点
	*
	* @param InActActionSequenceController 
	* @param InParentNode 父节点
	* @param InNodeName 节点名称
	*/
	FActActionSequenceTreeViewNode(const TSharedRef<FActActionSequenceController>& InActActionSequenceController, const TSharedPtr<FActActionSequenceTreeViewNode>& InParentNode = nullptr, FName InNodeName = NAME_None);
	virtual ~FActActionSequenceTreeViewNode();

	/**
	 * 是否为树的根节点
	 */
	bool IsTreeViewRoot();

	/**
	* @return FIX:A List of all Child nodes belonging to this node
	*/
	const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& GetChildNodes() const;

	FString GetPathName() const;
	/**
	* @return 是否被隐藏
	*/
	bool IsHidden() const;
	TSharedRef<SWidget> GenerateContainerWidgetForOutliner(const TSharedRef<SActActionSequenceTreeViewRow>& InRow);

	ActActionSequence::ESequenceNodeType GetType();
	/** @return Whether or not this node can be selected */
	bool IsSelectable() const;


	void SetParent(TSharedPtr<FActActionSequenceTreeViewNode> InParent, int32 DesiredChildIndex);

	/**
	* @return The parent of this node. Will return null if this node is part of the FSequencerNodeTree::GetRootNodes array.
	*/
	TSharedPtr<FActActionSequenceTreeViewNode> GetParent() const;

	/**
	* Get the display node that is ultimately responsible for constructing a section area widget for this node.
	* Could return this node itself, or a parent node
	*/
	TSharedPtr<FActActionSequenceTreeViewNode> GetSectionAreaAuthority() const;
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
	/**
	 * 获得当前资源使用的Tick帧率
	 */
	FFrameRate GetTickResolution() const;

	/**
	* @return The parent of this node, or the symbolic root node if this node is part of the FSequencerNodeTree::GetRootNodes array.
	*/
	TSharedPtr<FActActionSequenceTreeViewNode> GetParentOrRoot() const;
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

protected:
	/** FIX:List of children belonging to this node */
	TArray<TSharedRef<FActActionSequenceTreeViewNode>> ChildNodes;
	/** FIX:Parent tree that this node is in */
	// FActActionSequenceNodeTree& ParentNode;
	/** FIX:The name identifier of this node */
	FName NodeName;
	// /** The parent of this node*/
	// TWeakPtr<FActActionSequenceTreeViewNode> ParentNode;
	/** All of the sequencer sections in this node */
	TArray<TSharedRef<FActActionSequenceSectionBase>> Sections;
	/**
	 * 该节点的父节点，如果没有父节点则认为是树的根节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> ParentNode;
	/**
	 * 当前编辑的Sequence，即所有NodeTree所属的Sequence
	 */
	TWeakPtr<FActActionSequenceController> ActActionSequenceController;
};
