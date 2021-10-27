#pragma once

class SActImageTreeViewTableRow;
class SActImageTrackCarWidget;
class SActActionSequenceNotifyNode;

class SActImageTrackLaneWidget : public SCompoundWidget
{
public:
	// ** 使用 TPanelChildren 必须有的结构
	class Slot : public TSlotBase<Slot>
	{
	public:
		Slot(const TSharedRef<SActImageTrackLaneWidget>& InSlotContent);

		/** @return Get the vertical position of this slot inside its parent. */
		float GetVerticalOffset() const;

		EHorizontalAlignment HAlignment;// ** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Horizontal alignment for the slot.
		EVerticalAlignment VAlignment;  // ** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Vertical alignment for the slot.

	protected:
		TWeakPtr<SActImageTrackLaneWidget> SlotContent;// ** Slot 的具体 Widget 内容，这里是WeakPtr是因为基类有对 Widget 的管理
	};
	SLATE_BEGIN_ARGS(SActImageTrackLaneWidget) {}
	SLATE_END_ARGS()

	/** Construction from a track lane */
	// SActImageTrackLaneWidget(const TSharedRef<SActImageTreeViewTableRow>& InSequenceTreeViewNode);

	void Construct(const FArguments& InArgs);

	/** 构造TrackLane Widget*/
	// void MakeTrackLane();


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
	/** Get the desired physical vertical position of this track */
	float GetPhysicalPosition() const;
	/**
	 * 设置该节点对应的Widget的可见性
	 *
	 * @param InVisibility
	 */
	// void SetVisibility(EVisibility InVisibility);
protected:
	/** TreeViewNode Controller */
	TWeakPtr<SActImageTreeViewTableRow> SequenceTreeViewNode;
	/** The track lane that we represent. */
	TSharedPtr<SActImageTrackCarWidget> TrackLane;
	/** Notify Node */
	TSharedPtr<SActActionSequenceNotifyNode> NotifyNode;

	// FAnimNotifyEvent* AnimNotifyEvent;
public:
	TSharedRef<SActImageTreeViewTableRow> GetActActionSequenceTreeViewNode() const
	{
		return SequenceTreeViewNode.Pin().ToSharedRef();
	}

	TSharedRef<SActImageTrackCarWidget> GetActActionSequenceTrackLane() const
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
	FActActionCachedGeometry(TSharedRef<SActImageTrackLaneWidget> InTrack, float InTop, float InHeight)
		: Track(MoveTemp(InTrack)),
		  Top(InTop),
		  Height(InHeight) { }

	TSharedRef<SActImageTrackLaneWidget> Track;
	float Top;
	float Height;
};
