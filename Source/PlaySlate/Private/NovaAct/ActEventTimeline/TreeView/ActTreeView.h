#pragma once


#include "Common/NovaStruct.h"

class UActAnimation;
class SActTreeViewTrackCarWidget;
class SActTreeViewTrackAreaPanel;
class SActTreeViewNode;
class SActTreeViewHorizontalBox;

class SActTreeView : public STreeView<TSharedRef<SActTreeViewNode>>
{
public:
	typedef STreeView::FArguments FArguments;

	void Construct(const FArguments& InArgs, const TSharedRef<SActTreeViewTrackAreaPanel>& InTrackArea);

	
	/**
	 * 从InParent节点中获取所有的孩子节点，过滤掉隐藏的节点
	 * @param InParent
	 * @param OutChildren
	 */
	void OnGetChildren(TSharedRef<SActTreeViewNode> InParent, TArray<TSharedRef<SActTreeViewNode>>& OutChildren) const;

	/**
	 * 当展开状态改变的回调
	 * @param InDisplayNode
	 * @param bIsExpanded
	 */
	void OnExpansionChanged(TSharedRef<SActTreeViewNode> InDisplayNode, bool bIsExpanded);
	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();
	/**
	 * OnHitBoxesChanged多播事件回调，控制当前Viewport中的攻击盒
	 *
	 * @param InActAnimation
	 */
	void OnHitBoxesChanged(UActAnimation* InActAnimation);

	/**
	 * 查找或构造Folder类型的子节点
	 *
	 * @param InName Folder名称
	 * @return 查找或构造的子节点
	 */
	TSharedRef<SActTreeViewNode> FindOrCreateFolder(const FName& InName);


protected:
	/** Pointer to the node tree data that is used to populate this tree */
	// TWeakPtr<SActTreeViewNode> SequenceTreeViewNode;

	/** Strong pointer to the track area so we can generate track lanes as we need them */
	TSharedPtr<SActTreeViewTrackAreaPanel> ActTreeViewTrackAreaPanel;

	TArray<TSharedRef<SActTreeViewNode>> DisplayedRootNodes;// ** 从树的数据中复制和缓存的根节点信息

	TMap<FName, TSharedRef<SActTreeViewNode>> ChildNodes;// ** List of children belonging to this node
};
