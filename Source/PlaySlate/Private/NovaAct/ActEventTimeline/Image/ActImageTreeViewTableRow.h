#pragma once

#include "Common/NovaStruct.h"
#include "Common/NovaStaticFunction.h"

class IActImageTrackBase;
class SActImagePoolWidget;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActImageTreeView;
class SActImagePoolAreaPanel;
class SActImageTrackCarWidget;

using namespace NovaEnum;
using namespace NovaStruct;
/**
 * 基础的Sequence Node
 */
class SActImageTreeViewTableRow : public SMultiColumnTableRow<TSharedRef<SActImageTreeViewTableRow>>
{
public:
	typedef SMultiColumnTableRow::FArguments FArguments;

	SActImageTreeViewTableRow();
	void Construct(const FArguments& InArgs,
	               const TSharedRef<STableViewBase>& OwnerTableView,
	               const TSharedRef<IActImageTrackBase>& InActImageTrack);


	//~Begin SMultiColumnTableRow interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~End SMultiColumnTableRow interface

	/** 当Notify节点有改变时触发的回调 */
	void HandleNotifyChanged();

	/**
	 * @return 是否为树的根节点
	 */
	bool IsTreeViewRoot() const;

	/**
	 * @return A List of all Child nodes belonging to this node
	 */
	const TArray<TSharedRef<SActImageTreeViewTableRow>>& GetChildNodes() const;

	/**
	 * 根据Index顺序获取子节点
	 * 
	 * @param Index 输入顺序值
	 * @return 获取的子节点
	 */
	TSharedPtr<SActImageTreeViewTableRow> GetChildByIndex(int32 Index) const;

	/**
	 * @return 节点在树中的完整路径名
	 */
	FString GetPathName() const;

	/**
	 * @return 是否被隐藏
	 */
	bool IsHidden() const;

	/** @return Whether or not this node can be selected */
	bool IsSelectable() const;

	/**
	 * @return Whether this node should be displayed on the tree view
	 */
	bool IsVisible() const;
	/**
	 * 从树结构中移除自身，以及自身的所有子节点
	 */
	void RemoveFromParent();

	/**
	 * 重新挂载Parent并调整其在Parent中的节点顺序
	 *
	 * @param InParent 设置的Parent
	 * @param DesiredChildIndex 期望的节点位置，如果不设置则自动添加到尾部
	 */
	void SetParent(TSharedPtr<SActImageTreeViewTableRow> InParent, int32 DesiredChildIndex = INDEX_NONE);

	/**
	 * Get the display node that is ultimately responsible for constructing a section area widget for this node.
	 * Could return this node itself, or a parent node
	 */
	TSharedPtr<SActImageTreeViewTableRow> GetSectionAreaAuthority();

	/** TODO: */
	TArray<TSharedRef<FActActionSequenceSectionBase>>& GetSections();

	/** @return 获得当前节点的高度 */
	float GetNodeHeight() const;

	/** @return 是否能重命名*/
	bool CanRenameNode() const;

	/** @return The font used to draw the display name.*/
	FSlateFontInfo GetDisplayNameFont() const;

	/** @return 获得名称显示的颜色 */
	FSlateColor GetDisplayNameColor() const;

	/**
	 * @param NewDisplayName 待验证的名称
	 * @param OutErrorMessage 输出的错误信息
	 * @return 验证传入的名称是否符合要求，为true则符合要求，为false则错误信息不为空
	 */
	bool ValidateDisplayName(const FText& NewDisplayName, FText& OutErrorMessage) const;

	/**
	 * Set the node's display name.
	 *
	 * @param NewDisplayName the display name to set.
	 */
	void SetDisplayName(const FText& NewDisplayName);

	/**
	 * @return Gets an icon that represents this sequencer display node, This node's representative icon
	 */
	const FSlateBrush* GetIconBrush() const;

	/**
	 * @return Get a brush to overlay on top of the icon for this node, An overlay brush, or nullptr
	 */
	const FSlateBrush* GetIconOverlayBrush() const;

	/**
	 * @return Get the tooltip text to display for this node's icon, Text to display on the icon
	 */
	FText GetIconToolTipText() const;

	/**
	 * 将节点添加到显示中
	 */
	void AddDisplayNode(TSharedPtr<SActImageTreeViewTableRow> ChildTreeViewNode);

	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();
	/** 计算当前Track的纵向间距 */
	float ComputeTrackPosition();
protected:
	/** The legacy notify panel */
	TSharedPtr<SActImagePoolWidget> ActImageTrackPanel;
	/**
	 * 该节点的父节点，如果没有父节点则认为是树的根节点
	 */
	TSharedPtr<SActImageTreeViewTableRow> ParentNode;
	/** List of children belonging to this node */
	TArray<TSharedRef<SActImageTreeViewTableRow>> ChildNodes;
	/** All of the sequence sections in this node */
	TArray<TSharedRef<FActActionSequenceSectionBase>> Sections;
	/** The name identifier of this node */
	FName NodeName;
	/**
	 * Section区域的Widget
	 */
	TSharedPtr<SWidget> ActActionSectionWidget;
	/**
     * 这个Controller的Widget
     */
	TSharedPtr<SActImageTreeView> TreeView;
	/**
	 * Widget TreeView 对应的TrackArea，
	 */
	TSharedPtr<SActImagePoolAreaPanel> TrackArea;
	/**
	 * (Pinned)这个Controller的Widget
	 */
	TSharedPtr<SActImageTreeView> TreeViewPinned;
	/**
	 * (Pinned)Widget TreeView 对应的TrackArea
	 */
	TSharedPtr<SActImagePoolAreaPanel> TrackAreaPinned;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<TSharedRef<SActImageTreeViewTableRow>> DisplayedRootNodes;
	/** 当前节点的Outliner实际内容 */
	TSharedPtr<SWidget> OutlinerContent;
	/** TrackArea所使用的参数 */
	FActActionTrackAreaArgs ActActionTrackAreaArgs;
	/** The height of the track */
	float Height;
	// TODO:
	int32 PendingRenameTrackIndex;


public:
	// Table Row 对应的 Track 类型以及相关数据
	TSharedPtr<IActImageTrackBase> ActImageTrack;

	FActActionTrackAreaArgs& GetActActionTrackAreaArgs()
	{
		return ActActionTrackAreaArgs;
	}

	TArray<TSharedRef<SActImageTreeViewTableRow>>& GetDisplayedRootNodes()
	{
		if (IsTreeViewRoot())
		{
			return DisplayedRootNodes;
		}
		else
		{
			return GetParentNode()->GetDisplayedRootNodes();
		}
	}

	TSharedPtr<SActImageTreeViewTableRow> GetParentNode() const
	{
		return ParentNode;
	}

	TSharedRef<SWidget> GetActActionSectionWidget() const
	{
		return ActActionSectionWidget.ToSharedRef();
	}

	TSharedRef<SActImageTreeView> GetTreeView() const
	{
		return TreeView.ToSharedRef();
	}

	TSharedRef<SActImageTreeView> GetTreeViewPinned() const
	{
		return TreeViewPinned.ToSharedRef();
	}

	TSharedRef<SActImagePoolAreaPanel> GetTrackAreaPinned() const
	{
		return TrackAreaPinned.ToSharedRef();
	}

	TSharedRef<SActImagePoolAreaPanel> GetTrackArea() const
	{
		return TrackArea.ToSharedRef();
	}

	void SetHeight(float InHeight)
	{
		Height = InHeight;
	}

	/** Text to highlight when searching */
	TAttribute<FText> HighlightText;
};
