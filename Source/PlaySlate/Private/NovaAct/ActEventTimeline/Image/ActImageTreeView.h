#pragma once


#include "Common/NovaStruct.h"

class UActAnimation;
class SActImageTrackCarWidget;
class SActImageAreaPanel;
class SActImageTreeViewTableRow;
class SActImageHorizontalBox;
class SActImageTrackLaneWidget;
class SActImageTrackPanel;

class SActImageTreeView : public STreeView<TSharedRef<SActImageTreeViewTableRow>>
{
public:
	typedef STreeView::FArguments FArguments;

	virtual ~SActImageTreeView() override;
	// ReSharper disable once CppHidingFunction
	void Construct(const FArguments& InArgs, const TSharedRef<SActImageAreaPanel>& InActImageTrackAreaPanel);


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
	/**
	 * 依据特定的数据生成一行 TableRow，供TreeView回调使用，同时在TrackAreaPanel也生成一个对应的TrackLaneWidget 
	 * @param InTreeViewTableRow
	 * @param OwnerTable
	 * @return 
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<SActImageTreeViewTableRow> InTreeViewTableRow, const TSharedRef<STableViewBase>& OwnerTable);
protected:
	TSharedPtr<SActImageAreaPanel> ActImageAreaPanel;
	/** 从树的数据中复制和缓存的根节点信息 */
	TArray<TSharedRef<SActImageTreeViewTableRow>> DisplayedRootNodes;
	/** List of children belonging to this node */
	TArray<TSharedRef<SActImageTreeViewTableRow>> ChildNodes;
	/** A map of child slot content that exist in our view. */
	TMap<TSharedPtr<SActImageTreeViewTableRow>, TWeakPtr<SActImageTrackPanel>> TreeViewTableRow2TrackPanel;
	TMap<TSharedPtr<SActImageTrackPanel>, TWeakPtr<SActImageTreeViewTableRow>> TrackPanel2TreeViewTableRow;

	FDelegateHandle OnHitBoxesChangedHandle;
};
