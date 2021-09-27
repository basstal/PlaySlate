#pragma once
#include "ActActionSequenceTrackArea.h"
#include "SWidget/ActActionSequenceTreeView.h"
#include "Assets/ActActionSequenceNodeTree.h"
#include "Editor/ActActionSequenceDisplayNode.h"
#include "Utils/ActActionSequenceUtil.h"

typedef TSharedRef<FActActionSequenceDisplayNode> FActActionSequenceDisplayNodeRef;

class SActActionSequenceTreeViewRow : public SMultiColumnTableRow<FActActionSequenceDisplayNodeRef>
{
	DECLARE_DELEGATE_RetVal_ThreeParams(TSharedRef<SWidget>, OnGenerateWidgetForColumnDelegate, const FActActionSequenceDisplayNodeRef&, const FName&, const TSharedRef<SActActionSequenceTreeViewRow>&);
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTreeViewRow)
		{
		}

		/** FIX:Delegate to invoke to create a new column for this row */
		SLATE_EVENT(OnGenerateWidgetForColumnDelegate, OnGenerateWidgetForColumn)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, const FActActionSequenceDisplayNodeRef& InNode);
	//~Begin SMultiColumnTableRow interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~End SMultiColumnTableRow interface

	void AddTrackAreaReference(const TSharedRef<SActActionSequenceTrackLane>& Lane);

	TOptional<EItemDropZone> OnCanAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceDisplayNode> DisplayNode);
	FReply OnAcceptDrop(const FDragDropEvent& DragDropEvent, EItemDropZone InItemDropZone, TSharedRef<FActActionSequenceDisplayNode> DisplayNode);
	/** Gets the padding for this row based on whether it is a root node or not */
	FMargin GetRowPadding() const;

	

protected:
	/** The item associated with this row of data */
	TWeakPtr<FActActionSequenceDisplayNode> Node;

	/** Delegate to call to create a new widget for a particular column. */
	OnGenerateWidgetForColumnDelegate OnGenerateWidgetForColumn;
	TWeakPtr<SActActionSequenceTrackLane> TrackLaneReference;
};

/** Structure used to define a column in the tree view */
struct FActActionSequenceTreeViewColumn
{
	typedef TFunction<TSharedRef<SWidget>(const FActActionSequenceDisplayNodeRef&, const TSharedRef<SActActionSequenceTreeViewRow>&)> FOnGenerate;

	FActActionSequenceTreeViewColumn(const FOnGenerate& InOnGenerate, const TAttribute<float>& InWidth) : Generator(InOnGenerate), Width(InWidth)
	{
	}

	FActActionSequenceTreeViewColumn(FOnGenerate&& InOnGenerate, const TAttribute<float>& InWidth) : Generator(MoveTemp(InOnGenerate)), Width(InWidth)
	{
	}

	/** Function used to generate a cell for this column */
	FOnGenerate Generator;
	/** Attribute specifying the width of this column */
	TAttribute<float> Width;
};


class SActActionSequenceTreeView : public STreeView<FActActionSequenceDisplayNodeRef>
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTreeView)
		{
		}

		/** Externally supplied scroll bar */
		SLATE_ARGUMENT(TSharedPtr<SScrollBar>, ExternalScrollbar)
		/** FIX:Called to populate the context menu. */
		SLATE_EVENT(ActActionSequence::OnGetContextMenuContentDelegate, OnGetContextMenuContent)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceNodeTree>& InNodeTree, TSharedRef<SActActionSequenceTrackArea> InTrackArea);
	void SetupColumns(const FArguments& InArgs);

	//~Begin SWidget interface
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface

	void AddRootNodes(TSharedPtr<FActActionSequenceDisplayNode> SequenceDisplayNode);

	/**
	* 返回节点是否被过滤
	*
	* @param Node 检测是否被过滤的节点
	*/
	// bool IsNodeFiltered( const FActActionSequenceDisplayNodeRef Node ) const;

	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();
protected:
	// /**
	//  * 树结构Sequence的Track
	//  */
	// TSharedPtr<FActActionSequenceNodeTree> ActActionSequenceNodeTree;
	// /**
	//  * Track显示使用的Widget
	//  */
	// TSharedPtr<SActActionSequenceTrackArea> ActActionTrackAreaWidget;
	/**
	 * FIX:?
	 */
	ActActionSequence::OnGetContextMenuContentDelegate OnGetContextMenuContent;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<FActActionSequenceDisplayNodeRef> RootNodes;
	/** Strong pointer to the track area so we can generate track lanes as we need them */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;
	/** Pointer to the node tree data that is used to populate this tree */
	TSharedPtr<FActActionSequenceNodeTree> SequenceNodeTree;
	/**
	 * 依据特定的数据生成一行TableRow
	 */
	TSharedRef<ITableRow> OnGenerateRow(FActActionSequenceDisplayNodeRef InDisplayNode, const TSharedRef<STableViewBase>& OwnerTable);

	/**
	 * 为特定节点和列生成Widget
	 */
	TSharedRef<SWidget> GenerateWidgetFromColumn(const FActActionSequenceDisplayNodeRef& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row) const;
	/**
	 * 列定义，对应tree view中的列数据 
	 */
	TMap<FName, FActActionSequenceTreeViewColumn> Columns;

	/**
	 * 从InParent节点中获取所有的孩子节点
	 */
	void OnGetChildren(FActActionSequenceDisplayNodeRef InParent, TArray<FActActionSequenceDisplayNodeRef>& OutChildren) const;
	/**
	 * 所有被过滤的节点集合
	 */
	TSet<FActActionSequenceDisplayNodeRef> FilteredNodes;
};
