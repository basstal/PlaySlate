#pragma once

class SActImageTreeViewTableRow;
class SActImageTrackCarWidget;
class SActNotifyPoolNotifyNodeWidget;

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

		/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Horizontal alignment for the slot. */
		EHorizontalAlignment HAlignment;
		/** NOTE:必须是public的因为LayoutUtils.h在用这个字段，Vertical alignment for the slot. */
		EVerticalAlignment VAlignment;

	protected:
		/** Slot 的具体 Widget 内容，这里是WeakPtr是因为基类有对 Widget 的管理 */
		TSharedPtr<SActImageTrackLaneWidget> SlotContent;
	};
	SLATE_BEGIN_ARGS(SActImageTrackLaneWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow);

	//~Begin SCompoundWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args,
	                      const FGeometry& AllottedGeometry,
	                      const FSlateRect& MyCullingRect,
	                      FSlateWindowElementList& OutDrawElements,
	                      int32 LayerId,
	                      const FWidgetStyle& InWidgetStyle,
	                      bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;
	//~End SCompoundWidget interface

	/**
	 * 添加 TrackCar Widget 到轨道上
	 */
	void AddTrackCarWidget();

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
	// void GetTime(float& OutTime, int32& OutFrame);
	// /**
	//  * 获得当前节点状态持续时间
	//  *
	//  * @param OutTime 输出的时间，单位秒
	//  * @param OutFrame 输出的时间，单位帧
	//  */
	// void GetDuration(float& OutTime, int32& OutFrame);
	/** TODO: */
	bool IsBranchingPoint();
	/** 是否需要创建NotifyNode */
	bool HasNotifyNode();
	/** Get the desired physical vertical position of this track */
	float GetPhysicalPosition() const;
	/**
	 * this just refresh notify tracks - UI purpose only do not call this from here. This gets called by asset.
	 */
	void RefreshNotifyTracks();
	/**
	 * Called when a track changes it's selection; iterates all tracks collecting selected items
	 */
	void OnTrackSelectionChanged();
	/**
	 * 设置该节点对应的Widget的可见性
	 *
	 * @param InVisibility
	 */
	// void SetVisibility(EVisibility InVisibility);
protected:
	/** TreeViewNode Controller */
	TWeakPtr<SActImageTreeViewTableRow> ActImageTreeViewTableRow;
	// /** The track lane that we represent. */
	// TSharedPtr<SActImageTrackCarWidget> TrackLane;
	/** Notify Node */
	TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyNode;

	// FAnimNotifyEvent* AnimNotifyEvent;
	/** Get the height of this track */
	/** 所有已被添加的 TrackCar Widget */
	TArray<TSharedRef<SActImageTrackCarWidget>> ActImageTrackCarWidgets;
	/** TrackCar Widget Container */
	TSharedPtr<SBorder> ActImageTrackCarWidgetBorder;
	/** Recursion guard for selection */
	bool bIsSelecting;
public:
	float Height;

	TSharedRef<SActImageTreeViewTableRow> GetActActionSequenceTreeViewNode() const
	{
		return ActImageTreeViewTableRow.Pin().ToSharedRef();
	}

	//
	// TSharedRef<SActImageTrackCarWidget> GetActActionSequenceTrackLane() const
	// {
	// 	return TrackLane.ToSharedRef();
	// }

	// FActActionTrackAreaArgs& GetActActionTrackAreaArgs() const
	// {
	// 	return ActImageTreeViewTableRow.Pin()->GetActActionTrackAreaArgs();
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
