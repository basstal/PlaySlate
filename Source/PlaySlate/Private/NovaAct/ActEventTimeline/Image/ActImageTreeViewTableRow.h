﻿#pragma once

#include "ActImageTrackLaneWidget.h"
#include "Common/NovaStruct.h"
#include "Common/NovaStaticFunction.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"

class IActTreeViewTableRowBase;
class SActImageTrackLaneWidget;
class SActImageTrackPanel;
class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActImageTreeView;
class SActImageAreaPanel;
// class SActImageTrackLaneWidget;
class SActImageTrackCarWidget;

using namespace NovaEnum;
using namespace NovaStruct;
/**
 * 基础的Sequence Node
 */
class SActImageTreeViewTableRow : public SMultiColumnTableRow<TSharedRef<SActImageTreeViewTableRow>>
{
	// friend class SActActionOutlinerTreeNode;
public:
	typedef SMultiColumnTableRow::FArguments FArguments;

	SActImageTreeViewTableRow();
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& OwnerTableView, FName InNodeName = NAME_None, ENovaTreeViewTableRowType InNodeType = ENovaTreeViewTableRowType::None);

	
	//~Begin SMultiColumnTableRow interface
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& InColumnName) override;
	//~End SMultiColumnTableRow interface

	/** 当Notify节点有改变时触发的回调 */
	void HandleNotifyChanged();
	
	EActiveTimerReturnType HandlePendingRenameTimer(double InCurrentTime, float InDeltaTime, TWeakPtr<SInlineEditableTextBlock> InInlineEditableTextBlock);

	// /** TODO: */
	// TSharedRef<SActTrackPanel> GetAnimNotifyPanel();
	// /**
	//  * Generates a widget for display in the section area portion of the track area
	//  */
	// TSharedRef<SActTrackPanel> MakeWidgetForTrackArea();

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
	 * @return 当前节点的类型
	 */
	// ENovaTreeViewTableRowType GetType() const;

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

	/**
	 * 查找或构造Folder类型的子节点
	 *
	 * @param InName Folder名称
	 * @return 查找或构造的子节点
	 */
	TSharedRef<SActImageTreeViewTableRow> FindOrCreateFolder(const FName& InName);

	/**
	 * 设置节点为HitBox显示节点
	 *
	 * @param InHitBox 作为填充内容的数据
	 */
	void SetContentAsHitBox(FActActionHitBoxData& InHitBox);

	// /**
	//  * 设置节点的可见性
	//  *
	//  * @param InVisibility 可见性枚举
	//  */
	// void SetVisible(EVisibility InVisibility);
	/** 计算当前Track的纵向间距 */
	float ComputeTrackPosition();
	void Update();
	/** 获得根节点 */
	// TSharedPtr<SActImageTreeViewTableRow> GetRoot();
	/** 获得当前节点的可见性，节点可见性由Outliner的可见性决定 */
	// EVisibility GetVisibility() const;
	// /**
	//  * 为特定节点和列生成Widget
	//  * @param InNode
	//  * @param ColumnId
	//  * @param Row
	//  * @return 
	//  */
	// TSharedRef<SWidget> GenerateWidgetFromColumn(const TSharedRef<SActImageTreeViewTableRow>& InNode, const FName& ColumnId, const TSharedRef<SActActionSequenceTreeViewRow>& Row);

	ENovaTreeViewTableRowType GetTableRowType() const;

protected:
	/** The legacy notify panel */
	TSharedPtr<SActImageTrackPanel> ActImageTrackPanel;
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
	// /**
	//  * OutlinerTreeNode Widget
	//  */
	// TSharedPtr<SActActionOutlinerTreeNode> ActActionOutlinerTreeNode;
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
	TSharedPtr<SActImageAreaPanel> TrackArea;
	/**
	 * (Pinned)这个Controller的Widget
	 */
	TSharedPtr<SActImageTreeView> TreeViewPinned;
	/**
	 * (Pinned)Widget TreeView 对应的TrackArea
	 */
	TSharedPtr<SActImageAreaPanel> TrackAreaPinned;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<TSharedRef<SActImageTreeViewTableRow>> DisplayedRootNodes;
	/** 节点类型，决定节点的显示方式 */
	ENovaTreeViewTableRowType TableRowType;
	/** 当前节点的Outliner实际内容 */
	TSharedPtr<SWidget> OutlinerContent;
	/** TrackAreaSlot Controller */
	// TSharedPtr<SActImageTrackLaneWidget> ActNotifiesPanelLaneWidget;
	/** TrackArea所使用的参数 */
	FActActionTrackAreaArgs ActActionTrackAreaArgs;
	/** TODO:临时存这里 */
	FActActionHitBoxData* CachedHitBox;
	/** TrackAreaSlot 对应到 Geometry 信息 */
	TMap<TSharedRef<SActImageTrackLaneWidget>, FActActionCachedGeometry> CachedTrackGeometry;
	// /** 代表一行Track区域 */
	// TSharedPtr<SActTrackPanel> ActTrackPanel;
	/** The height of the track */
	float Height;
	// /** The outliner widget to allow for dynamic refresh */
	// TSharedPtr<SVerticalBox> OutlinerWidget;
	int32 PendingRenameTrackIndex;
	

	TSharedPtr<IActTreeViewTableRowBase> TreeViewTableRowComponent;
public:
	FName GetNodeName() const
	{
		return NodeName;
	}

	/** TODO:临时放到这里 */
	void SetHitBoxBegin(int32 InBegin)
	{
		if (CachedHitBox)
		{
			CachedHitBox->Begin = InBegin;
		}
	}

	void SetHitBoxDuration(int32 InDuration)
	{
		if (CachedHitBox)
		{
			CachedHitBox->End = InDuration - CachedHitBox->Begin;
		}
	}

	FActActionTrackAreaArgs& GetActActionTrackAreaArgs()
	{
		return ActActionTrackAreaArgs;
	}

	// TSharedRef<SActImageTrackLaneWidget> GetActActionTrackAreaSlot() const
	// {
	// 	return ActNotifiesPanelLaneWidget.ToSharedRef();
	// }

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

	// TSharedRef<SActActionOutlinerTreeNode> GetActActionOutlinerTreeNode() const
	// {
	// 	return ActActionOutlinerTreeNode.ToSharedRef();
	// }

	TSharedRef<SActImageTreeView> GetTreeView() const
	{
		return TreeView.ToSharedRef();
	}

	TSharedRef<SActImageTreeView> GetTreeViewPinned() const
	{
		return TreeViewPinned.ToSharedRef();
	}

	TSharedRef<SActImageAreaPanel> GetTrackAreaPinned() const
	{
		return TrackAreaPinned.ToSharedRef();
	}

	TSharedRef<SActImageAreaPanel> GetTrackArea() const
	{
		return TrackArea.ToSharedRef();
	}

	// TSharedRef<FActEventTimeline> GetSequenceController() const
	// {
	// 	return ActActionSequenceController.Pin().ToSharedRef();
	// }

	void SetHeight(float InHeight)
	{
		Height = InHeight;
	}
	/** Text to highlight when searching */
	TAttribute<FText> HighlightText;
};
