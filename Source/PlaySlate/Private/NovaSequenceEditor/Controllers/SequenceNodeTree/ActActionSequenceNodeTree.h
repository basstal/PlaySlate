#pragma once


class FActActionSequenceController;
class FActActionSequenceDisplayNode;

/**
 * 树节点为Sequence中所有可见节点，用来生成UI
 */
class FActActionSequenceNodeTree : public TSharedFromThis<FActActionSequenceNodeTree>
{
public:
	FActActionSequenceNodeTree(const TSharedRef<FActActionSequenceController>& InSequence);
	~FActActionSequenceNodeTree();

	/**
	 * 设置过滤的节点名称
	 */
	void SetFilterNodes(const FString& InFilter);
	/**
	 * 是否有激活的过滤器
	 */
	bool HasActiveFilter() const;
	/**
	* @return The root nodes of the tree
	*/
	const TArray<TSharedRef<FActActionSequenceDisplayNode>>& GetRootNodes() const;
protected:
	/**
	 * 是否需要刷新当前的过滤显示节点
	 */
	bool bNeedUpdateFilter;
	/**
	 * 当前的显示节点所使用的匹配字符串，过滤不符合的内容
	 */
	FString CurrentFilter;
	/**
	 * 当前编辑的Sequence，即所有NodeTree所属的Sequence
	 */
	TWeakPtr<FActActionSequenceController> Sequence;
	/** Symbolic root node that contains the actual displayed root nodes as children */
	TSharedPtr<FActActionSequenceDisplayNode> RootNode;
public:
	TSharedRef<FActActionSequenceController> GetSequence() const
	{
		return Sequence.Pin().ToSharedRef();
	}
};
