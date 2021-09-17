#pragma once

#include "Assets/ActActionSequenceNodeTree.h"

/**
 * 基础的Sequence Node
 */
class FActActionSequenceDisplayNode : public TSharedFromThis<FActActionSequenceDisplayNode>
{
public:
	FActActionSequenceDisplayNode();
	FActActionSequenceDisplayNode(FName InNodeName);
	virtual ~FActActionSequenceDisplayNode();


protected:
	/** FIX:List of children belonging to this node */
	TArray<TSharedRef<FActActionSequenceDisplayNode>> ChildNodes;
	/** FIX:Parent tree that this node is in */
	// FActActionSequenceNodeTree& ParentTree;
	/** FIX:The name identifier of this node */
	FName NodeName;
public:
	/**
	* @return FIX:A List of all Child nodes belonging to this node
	*/
	const TArray<TSharedRef<FActActionSequenceDisplayNode>>& GetChildNodes() const;

	/**
	* @return 是否被隐藏
	*/
	bool IsHidden() const;
};
