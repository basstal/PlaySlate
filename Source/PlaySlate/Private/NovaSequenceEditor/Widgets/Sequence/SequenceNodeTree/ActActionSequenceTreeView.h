#pragma once


#include "Utils/ActActionSequenceUtil.h"

class SActActionSequenceTrackLane;
class SActActionSequenceTrackArea;
class FActActionSequenceTreeViewNode;

class SActActionSequenceTreeView : public STreeView<TSharedRef<FActActionSequenceTreeViewNode>>
{
public:
SLATE_BEGIN_ARGS(SActActionSequenceTreeView)
		{
		}

		/** Externally supplied scroll bar */
		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceTreeViewNode>& InActActionSequenceTreeViewNode, TSharedRef<SActActionSequenceTrackArea> InTrackArea);
	/**
	 * 将节点添加到显示中
	 */
	void AddDisplayNode(TSharedPtr<FActActionSequenceTreeViewNode> SequenceDisplayNode);

	/**
	 * @param InDisplayNode
	 * @param OwnerTable
	 * @return 依据特定的数据生成一行TableRow 
	 */
	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable);

	/**
	 * 为特定节点和列生成Widget
	 * @param InNode
	 * @param ColumnId
	 * @param Row
	 * @return 
	 */
	TSharedRef<SWidget> GenerateWidgetFromColumn(const TSharedRef<FActActionSequenceTreeViewNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const;
	/**
	 * 从InParent节点中获取所有的孩子节点，过滤掉隐藏的节点
	 * @param InParent
	 * @param OutChildren
	 */
	void OnGetChildren(TSharedRef<FActActionSequenceTreeViewNode> InParent, TArray<TSharedRef<FActActionSequenceTreeViewNode>>& OutChildren) const;
	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();
protected:
	/** Pointer to the node tree data that is used to populate this tree */
	TWeakPtr<FActActionSequenceTreeViewNode> SequenceTreeViewNode;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<TSharedRef<FActActionSequenceTreeViewNode>> DisplayedRootNodes;
	/** Strong pointer to the track area so we can generate track lanes as we need them */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;
	/**
	 * 列定义，对应tree view中的列数据 
	 */
	TMap<FName, ActActionSequence::FActActionSequenceTreeViewColumn> Columns;
};
