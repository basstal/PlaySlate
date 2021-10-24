#pragma once

#include "IContentBrowserSingleton.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"

class FNovaActEditor;
class FActEventTimelineSlider;
class SActActionSequenceWidget;
class ASkeletalMeshActor;
class UActAnimation;
class FActActionSequenceTreeViewNode;
class SActActionViewportWidget;
class FActEventTimelineImage;

using namespace NovaStruct;
/**
 * COMMENT:
 */
class FActEventTimeline : public TSharedFromThis<FActEventTimeline>, FTickableEditorObject
{
public:
	FActEventTimeline(const TSharedRef<FNovaActEditor>& InNovaActEditor);
	virtual ~FActEventTimeline() override;

	/**
	 * 构造Sequence的Widget为SActActionSequenceWidget，同时初始化TreeView相关内容
	 */
	void Init();

	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~End FTickableEditorObject interface

	// /**
	//  * 
	//  */
	// void ExecuteTrackEditorCreateDelegate();

	/**
	 * 构建AddTrack菜单的内容
	 *
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	void BuildAddTrackMenu(FMenuBuilder& MenuBuilder);

	/**
	 * 设置AnimSequence的相关数据
	 *
	 * @param InActAnimation
	 */
	void OnAnimSequenceChanged(UActAnimation* InActAnimation);

	/**
	 * 设置Viewport的回放状态
	 *
	 * @param InPlaybackStatus 当前外部传入的播放状态
	 */
	void SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus);

	// /**
	//  * 将当前播放状态设置为Stopped
	//  */
	// void Pause();

	// /**
	//  * @return 返回当前的帧时间
	//  */
	// FFrameTime GetLocalFrameTime() const;

	// /**
	//  * @return 获得当前时间帧的显示文本
	//  */
	// FString GetFrameTimeText() const;

	/**
	 * 时间轴拖拽器开始拖拽的回调
	 */
	void OnBeginScrubberMovement();

	/**
	* 时间轴拖拽器结束拖拽的回调
	*/
	void OnEndScrubberMovement();

	// /**
	//  * 设置全局时间点到指定时间
	//  *
	//  * @param InFrameTime 设置的时间点
	//  */
	// void SetGlobalTime(FFrameTime InFrameTime) const;
	//
	// /**
	//  * 设置局部时间点到指定时间
	//  * 
	//  * @param InFrameTime 设置的时间点
	//  */
	// void SetLocalTimeDirectly(FFrameTime InFrameTime) const;

	/**
	 * 时间轴拖拽器位置改变的回调
	 */
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);

	// /**
	//  * @return 当前TimeSlider的显示范围
	//  */
	// TRange<float> GetViewRange() const;

	// /**
	//  * 设置当前的ViewRange
	//  * @param InViewRange 待设置的ViewRange
	//  * @param InViewRangeInterpolation TODO:
	//  */
	// void SetViewRange(TRange<float> InViewRange, ENovaViewRangeInterpolation InViewRangeInterpolation);
	//
	// /** @return 当前TimeSlider的最大显示范围 */
	// TRange<float> GetClampRange() const;

	/**
	 * 往AddTrack菜单中填充内容
	 * 
	 * @param MenuBuilder 被填充的菜单对象
	 */
	void PopulateAddMenuContext(FMenuBuilder& MenuBuilder);

	/**
	* OnHitBoxesChanged多播事件回调，控制当前Viewport中的攻击盒
	*
	* @param InActAnimation
	*/
	void OnHitBoxesChanged(UActAnimation* InActAnimation);

protected:
	/**
	 * 对Editor的弱引用，调用编辑器资源和相关工具方法
	 */
	TWeakPtr<FNovaActEditor> NovaActEditor;

	/**
	 * 所有已注册的CreateTrackEditor代理方法，在FActActionTrackEditorBase的子类中实现
	 */
	TArray<OnCreateTrackEditorDelegate> TrackEditorDelegates;

	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;

	// ENovaPlaybackType PlaybackState;// 当前的回放状态
	/**
	 * TimeSlider的Controller
	 */
	TSharedPtr<FActEventTimelineSlider> ActActionTimeSliderController;

	/**
	 * 所有可见节点DisplayedRootNodes的父节点，
	 * Sequence中所有可见根节点都储存在NodeTree中作为子节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> ActActionSequenceTreeViewNode;

	/**
	 * UMG Sequence main
	 */
	TSharedPtr<SActActionSequenceWidget> ActActionSequenceWidget;

	/** Numeric type interface used for converting parsing and generating strings from numbers */
	// TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
	// /**
	//  * TODO:这里要改成帧
	//  * Sequence TimeSlider 的显示的范围，这里的单位是秒
	//  */
	// TRange<float> TargetViewRange;
	//
	// /** Sequence TimeSlider 的最大显示范围，单位秒 */
	// TRange<float> TargetClampRange;

	TSharedPtr<TDataBindingSP<FActEventTimelineArgs>> ActEventTimelineArgsDB;    // EventTimeline 参数的数据绑定
	TSharedPtr<TDataBindingSP<FActEventTimelineEvents>> ActEventTimelineEventsDB;// EventTimeline 事件的数据绑定 TODO:这里也可以拆成参数
	FDelegateHandle OnHitBoxesChangedHandle;                                     // 攻击盒数据改变数据绑定的解绑凭证
	FDelegateHandle OnAnimSequenceChangedHandle;                                 // 数据绑定解绑

public:
	// ENovaPlaybackType GetPlaybackStatus() const
	// {
	// 	return PlaybackState;
	// }

	TSharedRef<SActActionSequenceWidget> GetActActionSequenceWidget() const
	{
		return ActActionSequenceWidget.ToSharedRef();
	}

	TSharedRef<FActActionSequenceTreeViewNode> GetActActionSequenceTreeViewRoot() const
	{
		return ActActionSequenceTreeViewNode.ToSharedRef();
	}

	TSharedRef<FActEventTimelineSlider> GetActActionTimeSliderController() const
	{
		return ActActionTimeSliderController.ToSharedRef();
	}

	TSharedRef<FNovaActEditor> GetActActionSequenceEditor() const
	{
		return NovaActEditor.Pin().ToSharedRef();
	}
};
