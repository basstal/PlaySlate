#pragma once


#include "Common/NovaStruct.h"

class UActAnimation;
class SActImageTrackCarWidget;
class SActImageTrackAreaPanel;
class SActImageTreeViewTableRow;
class SActImageHorizontalBox;

class SActImageTreeView : public STreeView<TSharedRef<SActImageTreeViewTableRow>>
{
public:
	typedef STreeView::FArguments FArguments;

	// ReSharper disable once CppHidingFunction
	void Construct(const FArguments& InArgs);


	/**
	 * 从InParent节点中获取所有的孩子节点，过滤掉隐藏的节点
	 * @param InParent
	 * @param OutChildren
	 */
	void OnGetChildren(TSharedRef<SActImageTreeViewTableRow> InParent, TArray<TSharedRef<SActImageTreeViewTableRow>>& OutChildren) const;

	/**
	 * 当展开状态改变的回调
	 * @param InDisplayNode
	 * @param bIsExpanded
	 */
	void OnExpansionChanged(TSharedRef<SActImageTreeViewTableRow> InDisplayNode, bool bIsExpanded);
	/**
	 * Refresh this tree as a result of the underlying tree data changing
	 */
	void Refresh();
	/**
	 * OnHitBoxesChanged多播事件回调，控制当前Viewport中的攻击盒
	 *
	 * @param InActAnimation
	 */
	void OnHitBoxesChanged(UActAnimation* InActAnimation);
protected:
	TArray<TSharedRef<SActImageTreeViewTableRow>> DisplayedRootNodes;// ** 从树的数据中复制和缓存的根节点信息

	TArray<TSharedRef<SActImageTreeViewTableRow>> ChildNodes;// ** List of children belonging to this node
};
