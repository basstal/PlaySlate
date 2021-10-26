#pragma once


#include "Common/NovaStruct.h"
#include "NovaAct/Assets/ActAnimation.h"

class SActActionSequenceTrackLane;
class SActActionSequenceTrackArea;
class FActActionSequenceTreeViewNode;
class SActTreeViewHorizontalBox;

class SActActionSequenceTreeView : public STreeView<TSharedRef<FActActionSequenceTreeViewNode>>
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTreeView) { }

		/** Externally supplied scroll bar */
		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SActActionSequenceTrackArea>& InTrackArea);

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

	/**
	 * 当展开状态改变的回调
	 * @param InDisplayNode
	 * @param bIsExpanded
	 */
	void OnExpansionChanged(TSharedRef<FActActionSequenceTreeViewNode> InDisplayNode, bool bIsExpanded);
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
	TSharedRef<FActActionSequenceTreeViewNode> FindOrCreateFolder(const FName& InName);

protected:
	/** Pointer to the node tree data that is used to populate this tree */
	// TWeakPtr<FActActionSequenceTreeViewNode> SequenceTreeViewNode;

	/** Strong pointer to the track area so we can generate track lanes as we need them */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;

	TArray<TSharedRef<FActActionSequenceTreeViewNode>> DisplayedRootNodes;// ** 从树的数据中复制和缓存的根节点信息

	TMap<FName, TSharedRef<FActActionSequenceTreeViewNode>> ChildNodes;// ** List of children belonging to this node
};
