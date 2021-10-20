#pragma once

#include "Utils/ActActionPlaybackUtil.h"
#include "Utils/ActActionSequenceUtil.h"

class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActActionSequenceTreeView;
class SActActionSequenceTrackArea;
class FActActionTrackAreaSlot;
class SActActionSequenceTrackLane;

/**
 * 基础的Sequence Node
 */
class FActActionSequenceTreeViewNode : public TSharedFromThis<FActActionSequenceTreeViewNode>
{
	friend class SActActionOutlinerTreeNode;

public:
	/**
	 * 构造一个树节点
	 *
	 * @param InActActionSequenceController 
	 * @param InNodeName 节点名称
	 * @param InNodeType 节点类型
	 */
	FActActionSequenceTreeViewNode(const TSharedRef<FActActionSequenceController>& InActActionSequenceController, FName InNodeName = NAME_None, ActActionSequence::ESequenceNodeType InNodeType = ActActionSequence::ESequenceNodeType::Root);

	virtual ~FActActionSequenceTreeViewNode();

	/**
	 * 构造节点的Widget
	 */
	void MakeActActionSequenceTreeView(const TSharedRef<SScrollBar>& ScrollBar);

	/**
	 * 构造Pinned节点的Widget
	 */
	void MakeActActionSequenceTreeViewPinned(const TSharedRef<SScrollBar>& ScrollBar);

	/**
	 * 构造节点的Outliner的Widget
	 *
	 * @param InRow 传入的内容Widget
	 * @return 输出包装用Outliner Widget
	 */
	TSharedRef<SActActionOutlinerTreeNode> MakeOutlinerWidget(const TSharedRef<SActActionSequenceTreeViewRow>& InRow);

	/**
	 * Generates a widget for display in the section area portion of the track area
	 */
	void MakeWidgetForSectionArea();

	/**
	 * @return 是否为树的根节点
	 */
	bool IsTreeViewRoot() const;

	/**
	 * @return A List of all Child nodes belonging to this node
	 */
	const TArray<TSharedRef<FActActionSequenceTreeViewNode>>& GetChildNodes() const;

	/**
	 * 根据Index顺序获取子节点
	 * 
	 * @param Index 输入顺序值
	 * @return 获取的子节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> GetChildByIndex(int Index) const;

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
	 * @return 当前节点的类型
	 */
	ActActionSequence::ESequenceNodeType GetType() const;

	/**
	 * 重新挂载Parent并调整其在Parent中的节点顺序
	 *
	 * @param InParent 设置的Parent
	 * @param DesiredChildIndex 期望的节点位置，如果不设置则自动添加到尾部
	 */
	void SetParent(TSharedPtr<FActActionSequenceTreeViewNode> InParent, int32 DesiredChildIndex = INDEX_NONE);

	/**
	 * Get the display node that is ultimately responsible for constructing a section area widget for this node.
	 * Could return this node itself, or a parent node
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> GetSectionAreaAuthority();

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
	void AddDisplayNode(TSharedPtr<FActActionSequenceTreeViewNode> ChildTreeViewNode);

	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();

	/**
	 * 查找或构造Folder类型的子节点
	 *
	 * @param InName Folder名称
	 * @return 查找或构造的子节点
	 */
	TSharedRef<FActActionSequenceTreeViewNode> FindOrCreateFolder(const FName& InName);

	/**
	 * 设置节点为HitBox显示节点
	 *
	 * @param InHitBox 作为填充内容的数据
	 */
	void SetContentAsHitBox(const FActActionHitBoxData& InHitBox);

	/**
	 * 设置节点的可见性
	 *
	 * @param bVisible 可见性枚举
	 */
	void SetVisible(EVisibility bVisible);
	/** 计算当前Track的纵向间距 */
	float ComputeTrackPosition();
	/** 获得根节点 */
	TSharedPtr<FActActionSequenceTreeViewNode> GetRoot();
protected:
	/**
	 * 当前编辑的Sequence，即所有NodeTree所属的Sequence
	 */
	TWeakPtr<FActActionSequenceController> ActActionSequenceController;
	/**
	 * 该节点的父节点，如果没有父节点则认为是树的根节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> ParentNode;
	/** List of children belonging to this node */
	TArray<TSharedRef<FActActionSequenceTreeViewNode>> ChildNodes;
	/** All of the sequence sections in this node */
	TArray<TSharedRef<FActActionSequenceSectionBase>> Sections;
	/** The name identifier of this node */
	FName NodeName;
	/**
	 * OutlinerTreeNode Widget
	 */
	TSharedPtr<SActActionOutlinerTreeNode> ActActionOutlinerTreeNode;
	/**
	 * Section区域的Widget
	 */
	TSharedPtr<SWidget> ActActionSectionWidget;
	/**
     * 这个Controller的Widget
     */
	TSharedPtr<SActActionSequenceTreeView> TreeView;
	/**
	 * Widget TreeView 对应的TrackArea，
	 */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;
	/**
	 * (Pinned)这个Controller的Widget
	 */
	TSharedPtr<SActActionSequenceTreeView> TreeViewPinned;
	/**
	 * (Pinned)Widget TreeView 对应的TrackArea
	 */
	TSharedPtr<SActActionSequenceTrackArea> TrackAreaPinned;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<TSharedRef<FActActionSequenceTreeViewNode>> DisplayedRootNodes;
	/** 节点类型 */
	ActActionSequence::ESequenceNodeType NodeType;
	/** 当前节点的Outliner实际内容 */
	TSharedPtr<SWidget> OutlinerContent;
	/** TrackAreaSlot Controller */
	TSharedPtr<FActActionTrackAreaSlot> ActActionTrackAreaSlot;
	/** TrackArea所使用的参数 */
	ActActionSequence::FActActionTrackAreaArgs ActActionTrackAreaArgs;
	/** TODO:临时存这里 */
	FActActionHitBoxData CachedHitBox;
	/** TrackAreaSlot 对应到 Geometry 信息 */
	TMap<TSharedRef<FActActionTrackAreaSlot>, ActActionSequence::FActActionCachedGeometry> CachedTrackGeometry;
public:
	ActActionSequence::FActActionTrackAreaArgs& GetActActionTrackAreaArgs()
	{
		return ActActionTrackAreaArgs;
	}

	TSharedRef<FActActionTrackAreaSlot> GetActActionTrackAreaSlot() const
	{
		return ActActionTrackAreaSlot.ToSharedRef();
	}

	TArray<TSharedRef<FActActionSequenceTreeViewNode>>& GetDisplayedRootNodes()
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

	TSharedPtr<FActActionSequenceTreeViewNode> GetParentNode() const
	{
		return ParentNode;
	}

	TSharedRef<SWidget> GetActActionSectionWidget() const
	{
		return ActActionSectionWidget.ToSharedRef();
	}

	TSharedRef<SActActionOutlinerTreeNode> GetActActionOutlinerTreeNode() const
	{
		return ActActionOutlinerTreeNode.ToSharedRef();
	}

	TSharedRef<SActActionSequenceTreeView> GetTreeView() const
	{
		return TreeView.ToSharedRef();
	}

	TSharedRef<SActActionSequenceTreeView> GetTreeViewPinned() const
	{
		return TreeViewPinned.ToSharedRef();
	}

	TSharedRef<SActActionSequenceTrackArea> GetTrackAreaPinned() const
	{
		return TrackAreaPinned.ToSharedRef();
	}

	TSharedRef<SActActionSequenceTrackArea> GetTrackArea() const
	{
		return TrackArea.ToSharedRef();
	}

	TSharedRef<FActActionSequenceController> GetSequenceController() const
	{
		return ActActionSequenceController.Pin().ToSharedRef();
	}
};
