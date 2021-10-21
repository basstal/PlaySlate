#pragma once
#include "ActActionSequenceTreeViewNode.h"

class SActActionSequenceTrackLane;
class FActActionSequenceTreeViewNode;
/**
* Structure representing a slot in the track area.
*/
class FActActionTrackAreaSlot : public TSlotBase<FActActionTrackAreaSlot>, public TSharedFromThis<FActActionTrackAreaSlot>
{
public:
	/** Construction from a track lane */
	FActActionTrackAreaSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InSequenceTreeViewNode);

	/** 构造TrackLane Widget*/
	void MakeTrackLane();
	/** Get the vertical position of this slot inside its parent. */
	float GetVerticalOffset() const;

	/** 获得节点的Tooltip */
	FText GetNodeTooltip();

	/** 获得动画总时长 */
	float GetPlayLength();
	/** 获得描述文本 */
	FName GetName();
	/** 获得编辑器颜色 */
	FLinearColor GetEditorColor();
	// /**
	//  * 获得开始时间
	//  *
	//  * @param OutTime 输出的时间，单位秒
	//  * @param OutFrame 输出的时间，单位帧
	//  */
	// void GetTime(float& OutTime, int& OutFrame);
	// /**
	//  * 获得当前节点状态持续时间
	//  *
	//  * @param OutTime 输出的时间，单位秒
	//  * @param OutFrame 输出的时间，单位帧
	//  */
	// void GetDuration(float& OutTime, int& OutFrame);
	/** TODO: */
	bool IsBranchingPoint();
	/** 是否需要创建NotifyNode */
	bool HasNotifyNode();
	/**
	 * 设置该节点对应的Widget的可见性
	 *
	 * @param InVisibility
	 */
	void SetVisibility(EVisibility InVisibility);
	/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Horizontal alignment for the slot. */
	EHorizontalAlignment HAlignment;
	/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Vertical alignment for the slot. */
	EVerticalAlignment VAlignment;
protected:
	/** TreeViewNode Controller */
	TWeakPtr<FActActionSequenceTreeViewNode> SequenceTreeViewNode;
	/** The track lane that we represent. */
	TSharedPtr<SActActionSequenceTrackLane> TrackLane;
	/** Notify Node */
	TSharedPtr<SActActionSequenceNotifyNode> NotifyNode;

	// FAnimNotifyEvent* AnimNotifyEvent;
public:
	TSharedRef<FActActionSequenceTreeViewNode> GetActActionSequenceTreeViewNode() const
	{
		return SequenceTreeViewNode.Pin().ToSharedRef();
	}

	TSharedRef<SActActionSequenceTrackLane> GetActActionSequenceTrackLane() const
	{
		return TrackLane.ToSharedRef();
	}

	ActActionSequence::FActActionTrackAreaArgs& GetActActionTrackAreaArgs() const
	{
		return SequenceTreeViewNode.Pin()->GetActActionTrackAreaArgs();
	}

	// FAnimNotifyEvent* GetNotifyEvent() const
	// {
	// 	return AnimNotifyEvent;
	// }

	
};
