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
	/** TODO: */
	float GetTime();
	/** TODO: */
	float GetDuration();
	/** TODO: */
	bool IsBranchingPoint();

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

	const float NotifyHeightOffset = 0.f;
	const float NotifyHeight = 24.0f;
};
