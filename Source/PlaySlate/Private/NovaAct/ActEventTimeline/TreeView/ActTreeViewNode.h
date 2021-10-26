#pragma once

#include "Common/NovaStruct.h"
#include "NovaAct/Assets/ActActionSequenceStructs.h"

class FActActionSequenceSectionBase;
class SActActionSequenceTreeViewRow;
class SActActionOutlinerTreeNode;
class SActTreeView;
class SActTreeViewTrackAreaPanel;
class SActTreeViewTrackLaneWidget;
class SActTreeViewTrackCarWidget;

using namespace NovaEnum;
using namespace NovaStruct;
/**
 * 基础的Sequence Node
 */
class SActTreeViewNode : public SCompoundWidget
{
	friend class SActActionOutlinerTreeNode;

public:
	/**
	 * 构造一个树节点
	 *
	 * @param InNodeName 节点名称
	 * @param InNodeType 节点类型
	 */
	SActTreeViewNode(FName InNodeName = NAME_None, ENovaSequenceNodeType InNodeType = ENovaSequenceNodeType::Root);

	virtual ~SActTreeViewNode() override;

	// /**
	//  * 构造节点的Widget
	//  */
	// void MakeActActionSequenceTreeView(const TSharedRef<SScrollBar>& ScrollBar);
	//
	// /**
	//  * 构造Pinned节点的Widget
	//  */
	// void MakeActActionSequenceTreeViewPinned(const TSharedRef<SScrollBar>& ScrollBar);

	/**
	 * 构造节点的Outliner的Widget
	 *
	 * @param InRow 传入的内容Widget
	 * @return 输出包装用Outliner Widget
	 */
	TSharedRef<SWidget> MakeOutlinerWidget(const TSharedRef<SActActionSequenceTreeViewRow>& InRow);
	// TSharedRef<SActTrackPanel> GetActTrackPanel();

	/** 当Notify节点有改变时触发的回调 */
	void HandleNotifyChanged();
	void RefreshOutlinerWidget();
	void OnCommitTrackName(const FText& InText, ETextCommit::Type CommitInfo, int32 TrackIndexToName);
	TSharedRef<SWidget> BuildNotifiesPanelSubMenu(int32 InTrackIndex);
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
	const TArray<TSharedRef<SActTreeViewNode>>& GetChildNodes() const;

	/**
	 * 根据Index顺序获取子节点
	 * 
	 * @param Index 输入顺序值
	 * @return 获取的子节点
	 */
	TSharedPtr<SActTreeViewNode> GetChildByIndex(int Index) const;

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
	ENovaSequenceNodeType GetType() const;

	/**
	 * 重新挂载Parent并调整其在Parent中的节点顺序
	 *
	 * @param InParent 设置的Parent
	 * @param DesiredChildIndex 期望的节点位置，如果不设置则自动添加到尾部
	 */
	void SetParent(TSharedPtr<SActTreeViewNode> InParent, int32 DesiredChildIndex = INDEX_NONE);

	/**
	 * Get the display node that is ultimately responsible for constructing a section area widget for this node.
	 * Could return this node itself, or a parent node
	 */
	TSharedPtr<SActTreeViewNode> GetSectionAreaAuthority();

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
	void AddDisplayNode(TSharedPtr<SActTreeViewNode> ChildTreeViewNode);

	/** Refresh this tree as a result of the underlying tree data changing */
	void Refresh();

	/**
	 * 查找或构造Folder类型的子节点
	 *
	 * @param InName Folder名称
	 * @return 查找或构造的子节点
	 */
	TSharedRef<SActTreeViewNode> FindOrCreateFolder(const FName& InName);

	/**
	 * 设置节点为HitBox显示节点
	 *
	 * @param InHitBox 作为填充内容的数据
	 */
	void SetContentAsHitBox(FActActionHitBoxData& InHitBox);

	/**
	 * 设置节点的可见性
	 *
	 * @param InVisibility 可见性枚举
	 */
	void SetVisible(EVisibility InVisibility);
	/** 计算当前Track的纵向间距 */
	float ComputeTrackPosition();
	/** 获得根节点 */
	// TSharedPtr<SActTreeViewNode> GetRoot();
	/** 获得当前节点的可见性，节点可见性由Outliner的可见性决定 */
	// EVisibility GetVisibility() const;
protected:
	/**
	 * 该节点的父节点，如果没有父节点则认为是树的根节点
	 */
	TSharedPtr<SActTreeViewNode> ParentNode;
	/** List of children belonging to this node */
	TArray<TSharedRef<SActTreeViewNode>> ChildNodes;
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
	TSharedPtr<SActTreeView> TreeView;
	/**
	 * Widget TreeView 对应的TrackArea，
	 */
	TSharedPtr<SActTreeViewTrackAreaPanel> TrackArea;
	/**
	 * (Pinned)这个Controller的Widget
	 */
	TSharedPtr<SActTreeView> TreeViewPinned;
	/**
	 * (Pinned)Widget TreeView 对应的TrackArea
	 */
	TSharedPtr<SActTreeViewTrackAreaPanel> TrackAreaPinned;
	/**
	 * 从树的数据中复制和缓存的根节点信息
	 */
	TArray<TSharedRef<SActTreeViewNode>> DisplayedRootNodes;
	/** 节点类型 */
	ENovaSequenceNodeType NodeType;
	/** 当前节点的Outliner实际内容 */
	TSharedPtr<SWidget> OutlinerContent;
	/** TrackAreaSlot Controller */
	// TSharedPtr<SActTreeViewTrackLaneWidget> ActActionTrackAreaSlot;
	/** TrackArea所使用的参数 */
	FActActionTrackAreaArgs ActActionTrackAreaArgs;
	/** TODO:临时存这里 */
	FActActionHitBoxData* CachedHitBox;
	/** TrackAreaSlot 对应到 Geometry 信息 */
	TMap<TSharedRef<SActTreeViewTrackLaneWidget>, FActActionCachedGeometry> CachedTrackGeometry;
	// /** 代表一行Track区域 */
	// TSharedPtr<SActTrackPanel> ActTrackPanel;
	/** The height of the track */
	float Height;
	/** The outliner widget to allow for dynamic refresh */
	TSharedPtr<SVerticalBox> OutlinerWidget;
	int32 PendingRenameTrackIndex;
public:
	/** TODO:临时放到这里 */
	void SetHitBoxBegin(int InBegin)
	{
		if (CachedHitBox)
		{
			CachedHitBox->Begin = InBegin;
		}
	}

	void SetHitBoxDuration(int InDuration)
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

	// TSharedRef<SActTreeViewTrackLaneWidget> GetActActionTrackAreaSlot() const
	// {
	// 	return ActActionTrackAreaSlot.ToSharedRef();
	// }

	TArray<TSharedRef<SActTreeViewNode>>& GetDisplayedRootNodes()
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

	TSharedPtr<SActTreeViewNode> GetParentNode() const
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

	TSharedRef<SActTreeView> GetTreeView() const
	{
		return TreeView.ToSharedRef();
	}

	TSharedRef<SActTreeView> GetTreeViewPinned() const
	{
		return TreeViewPinned.ToSharedRef();
	}

	TSharedRef<SActTreeViewTrackAreaPanel> GetTrackAreaPinned() const
	{
		return TrackAreaPinned.ToSharedRef();
	}

	TSharedRef<SActTreeViewTrackAreaPanel> GetTrackArea() const
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
};
