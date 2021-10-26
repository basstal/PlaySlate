#pragma once

class SActTreeViewNode;
class SActTreeViewTrackCarWidget;
class SActActionSequenceNotifyNode;

class SActTreeViewTrackLaneWidget : public TSlotBase<SActTreeViewTrackLaneWidget>, public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActTreeViewTrackLaneWidget) {}
	SLATE_END_ARGS()

	SActTreeViewTrackLaneWidget();
	/** Construction from a track lane */
	// SActTreeViewTrackLaneWidget(const TSharedRef<SActTreeViewNode>& InSequenceTreeViewNode);

	void Construct(const FArguments& InArgs);

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
	// void SetVisibility(EVisibility InVisibility);
	/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Horizontal alignment for the slot. */
	EHorizontalAlignment HAlignment;
	/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Vertical alignment for the slot. */
	EVerticalAlignment VAlignment;
protected:
	/** TreeViewNode Controller */
	TWeakPtr<SActTreeViewNode> SequenceTreeViewNode;
	/** The track lane that we represent. */
	TSharedPtr<SActTreeViewTrackCarWidget> TrackLane;
	/** Notify Node */
	TSharedPtr<SActActionSequenceNotifyNode> NotifyNode;

	// FAnimNotifyEvent* AnimNotifyEvent;
public:
	TSharedRef<SActTreeViewNode> GetActActionSequenceTreeViewNode() const
	{
		return SequenceTreeViewNode.Pin().ToSharedRef();
	}

	TSharedRef<SActTreeViewTrackCarWidget> GetActActionSequenceTrackLane() const
	{
		return TrackLane.ToSharedRef();
	}

	// FActActionTrackAreaArgs& GetActActionTrackAreaArgs() const
	// {
	// 	return SequenceTreeViewNode.Pin()->GetActActionTrackAreaArgs();
	// }

	// FAnimNotifyEvent* GetNotifyEvent() const
	// {
	// 	return AnimNotifyEvent;
	// }
};

// ** TODO:这里无法放到NovaStruct中因为会导致 模糊的SActTreeViewTrackLaneWidget
/** Structure used to cache physical geometry for a particular track */
struct FActActionCachedGeometry
{
	FActActionCachedGeometry(TSharedRef<SActTreeViewTrackLaneWidget> InTrack, float InTop, float InHeight)
		: Track(MoveTemp(InTrack)),
		  Top(InTop),
		  Height(InHeight) { }

	TSharedRef<SActTreeViewTrackLaneWidget> Track;
	float Top;
	float Height;
};
