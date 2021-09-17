#include "ActActionSequenceDisplayNode.h"

FActActionSequenceDisplayNode::FActActionSequenceDisplayNode()
{
}

FActActionSequenceDisplayNode::FActActionSequenceDisplayNode(FName InNodeName)
	: NodeName(InNodeName)
{
}

FActActionSequenceDisplayNode::~FActActionSequenceDisplayNode()
{
}

const TArray<TSharedRef<FActActionSequenceDisplayNode>>& FActActionSequenceDisplayNode::GetChildNodes() const
{
	return ChildNodes;
}

bool FActActionSequenceDisplayNode::IsHidden() const
{
	// return ParentTree.HasActiveFilter() && !ParentTree.IsNodeFiltered(AsShared());
	return false;
}
