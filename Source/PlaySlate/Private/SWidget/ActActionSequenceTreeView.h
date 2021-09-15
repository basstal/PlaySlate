#pragma once
#include "SWidget/ActActionSequenceTreeView.h"
#include "SWidget/ActActionSequenceTreeViewRow.h"
#include "SWidget/ActActionTrackArea.h"
#include "Assets/ActActionSequenceNodeTree.h"
#include "Editor/ActActionSequenceDisplayNode.h"

// DECLARE_DELEGATE_OneParam(FOnGetContextMenuContent, FMenuBuilder&);

// /** Structure used to define a column in the tree view */
// struct FActActionSequenceTreeViewColumn
// {
// 	typedef TFunction<TSharedRef<SWidget>(const TSharedRef<FActActionSequenceDisplayNode>&, const TSharedRef<SActActionSequenceTreeViewRow>&)> FOnGenerate;
//
// 	FActActionSequenceTreeViewColumn(const FOnGenerate& InOnGenerate, const TAttribute<float>& InWidth) : Generator(InOnGenerate), Width(InWidth) {}
// 	FActActionSequenceTreeViewColumn(FOnGenerate&& InOnGenerate, const TAttribute<float>& InWidth) : Generator(MoveTemp(InOnGenerate)), Width(InWidth) {}
//
// 	/** FIX:Function used to generate a cell for this column */
// 	FOnGenerate Generator;
// 	/** FIX:Attribute specifying the width of this column */
// 	TAttribute<float> Width;
// };

// class SActActionSequenceTreeView : public STreeView<TSharedRef<FActActionSequenceDisplayNode>>
// {
// public:
// 	SLATE_BEGIN_ARGS(SActActionSequenceTreeView){}
// 		/** FIX:Called to populate the context menu. */
// 		SLATE_EVENT(FOnGetContextMenuContent, OnGetContextMenuContent)
// 	SLATE_END_ARGS()
//
// 	void Construct(const FArguments& InArgs, TSharedPtr<FActActionSequenceNodeTree> InNodeTree, TSharedPtr<SActActionTrackArea> InTrackArea);
//
//
// 	/**
// 	* 返回节点是否被过滤
// 	*
// 	* @param Node 检测是否被过滤的节点
// 	*/
// 	bool IsNodeFiltered( const TSharedRef<FActActionSequenceDisplayNode> Node ) const;
// protected:
// 	/**
// 	 * 树结构Sequence的Track
// 	 */
// 	TSharedPtr<FActActionSequenceNodeTree> ActActionSequenceNodeTree;
// 	/**
// 	 * Track显示使用的Widget
// 	 */
// 	TSharedPtr<SActActionTrackArea> ActActionTrackAreaWidget;
//
// 	/**
// 	 * FIX:?
// 	 */
// 	FOnGetContextMenuContent OnGetContextMenuContent;
// 	/**
// 	 * 从树的数据中复制和缓存的根节点信息
// 	 */
// 	TArray<TSharedRef<FActActionSequenceDisplayNode>> RootNodes;
//
// 	/**
// 	 * 依据特定的数据生成一行TableRow
// 	 */
// 	TSharedRef<ITableRow> OnGenerateRow(TSharedRef<FActActionSequenceDisplayNode> InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable);
//
// 	/**
// 	 * 为特定节点和列生成Widget
// 	 */
// 	TSharedRef<SWidget> GenerateWidgetFromColumn(const TSharedRef<FActActionSequenceDisplayNode>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const;
// 	/**
// 	 * 列定义，对应tree view中的列数据 
// 	 */
// 	TMap<FName, FActActionSequenceTreeViewColumn> Columns;
//
// 	/**
// 	 * 从InParent节点中获取所有的孩子节点
// 	 */
// 	void OnGetChildren(TSharedRef<FActActionSequenceDisplayNode> InParent, TArray<TSharedRef<FActActionSequenceDisplayNode>>& OutChildren) const;
// 	/**
// 	 * 所有被过滤的节点集合
// 	 */
// 	TSet< TSharedRef<FActActionSequenceDisplayNode> > FilteredNodes;
// 	
// 	
// };
