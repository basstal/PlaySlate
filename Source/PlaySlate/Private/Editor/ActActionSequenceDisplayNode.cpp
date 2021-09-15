// #include "ActActionSequenceDisplayNode.h"
//
// #include "SWidget/ActActionSequenceTreeView.h"
//
// FActActionSequenceDisplayNode::FActActionSequenceDisplayNode(FName InNodeName, FActActionSequenceNodeTree& InParentTree)
// 	: ParentTree(InParentTree),
// 	  NodeName(InNodeName)
// {
// }
//
// FActActionSequenceDisplayNode::~FActActionSequenceDisplayNode()
// {
// }
//
// const TArray<TSharedRef<FActActionSequenceDisplayNode>>& FActActionSequenceDisplayNode::GetChildNodes() const
// {
// 	return ChildNodes;
// }
//
// bool FActActionSequenceDisplayNode::IsHidden() const
// {
// 	// return ParentTree.HasActiveFilter() && !ParentTree.IsNodeFiltered(AsShared());
// 	return false;
// }
