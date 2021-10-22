#pragma once

#include "IContentBrowserSingleton.h"
#include "Common/NovaStruct.h"

class FNovaActEditor;
class FActEventTimelineSlider;
class SActActionSequenceWidget;
class ASkeletalMeshActor;
class UActAnimation;
class FActActionSequenceTreeViewNode;
class SActActionViewportWidget;
class FActEventTimelineImage;

/**
 * Sequence Tab的主要控制器
 * 对应的View模块为SActActionSequenceWidget
 */
class FActEventTimelineBrain : public TSharedFromThis<FActEventTimelineBrain>, FTickableEditorObject
{
public:
	FActEventTimelineBrain(const TSharedRef<FNovaActEditor>& InActActionSequenceEditor);

	virtual ~FActEventTimelineBrain() override;

	/**
	* 构造Sequence的Widget为SActActionSequenceWidget，同时初始化TreeView相关内容
	*
	* @param ViewParams 构造Widget使用的相关参数
	*/
	void MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams);

	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;

	virtual TStatId GetStatId() const override;

	//~End FTickableEditorObject interface

	/**
	 * 调用已注册的TrackEditor的Create代理，并收集创建的TrackEditor实例
	 */
	void ExecuteTrackEditorCreateDelegate();

	/**
	 * 构建AddTrack菜单的内容
	 *
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	void BuildAddTrackMenu(FMenuBuilder& MenuBuilder);

	/** @param InAnimSequence 被添加的AnimSequence实例 */
	void AddAnimSequenceTrack(UAnimSequence* InAnimSequence);

	/**
	 * 设置当前的播放状态
	 *
	 * @param InPlaybackStatus 设置为该播放状态
	 */
	void SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus);

	/**
	 * 将当前播放状态设置为Stopped
	 */
	void Pause();

	/**
	 * @return 返回当前的帧时间
	 */
	FFrameTime GetLocalFrameTime() const;

	/**
	 * @return 获得当前时间帧的显示文本
	 */
	FString GetFrameTimeText() const;

	/**
	 * 时间轴拖拽器开始拖拽的回调
	 */
	void OnBeginScrubbing();

	/**
	* 时间轴拖拽器结束拖拽的回调
	*/
	void OnEndScrubbing();

	/**
	 * 设置全局时间点到指定时间
	 *
	 * @param InFrameTime 设置的时间点
	 */
	void SetGlobalTime(FFrameTime InFrameTime) const;

	/**
	 * 设置局部时间点到指定时间
	 * 
	 * @param InFrameTime 设置的时间点
	 */
	void SetLocalTimeDirectly(FFrameTime InFrameTime) const;

	/**
	 * 时间轴拖拽器位置改变的回调
	 */
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);

	/**
	 * @return 当前TimeSlider的显示范围
	 */
	ActActionSequence::FActActionAnimatedRange GetViewRange() const;

	/**
	 * 设置当前的ViewRange
	 * @param InViewRange 待设置的ViewRange
	 * @param InViewRangeInterpolation TODO:
	 */
	void SetViewRange(TRange<double> InViewRange, ENovaViewRangeInterpolation InViewRangeInterpolation);

	/** @return 当前TimeSlider的最大显示范围 */
	ActActionSequence::FActActionAnimatedRange GetClampRange() const;

	/**
	 * 往AddTrack菜单中填充内容
	 * 
	 * @param MenuBuilder 被填充的菜单对象
	 */
	void PopulateAddMenuContext(FMenuBuilder& MenuBuilder);

	/**
	* OnHitBoxesChanged多播事件回调，控制当前Viewport中的攻击盒
	*
	* @param InHitBoxData 传入的攻击盒数据
	*/
	void OnHitBoxesChanged(TArray<FActActionHitBoxData>& InHitBoxData);

protected:
	/**
	 * 对Editor的弱引用，调用编辑器资源和相关工具方法
	 */
	TWeakPtr<FNovaActEditor> ActActionSequenceEditor;

	/**
	 * 所有已注册的CreateTrackEditor代理方法，在FActActionTrackEditorBase的子类中实现
	 */
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> TrackEditorDelegates;

	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;

	/**
	 * 当前的播放状态
	 */
	ENovaPlaybackType PlaybackState;

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
	TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;

	/**
	 * TimeSlider的控制相关参数
	 */
	ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs;

	/**
	 * TODO:这里要改成帧
	 * Sequence TimeSlider 的显示的范围，这里的单位是秒
	 */
	TRange<double> TargetViewRange;

	/** Sequence TimeSlider 的最大显示范围，单位秒 */
	TRange<double> TargetClampRange;

public:
	ENovaPlaybackType GetPlaybackStatus() const
	{
		return PlaybackState;
	}

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
		return ActActionSequenceEditor.Pin().ToSharedRef();
	}

	ActActionSequence::FActActionTimeSliderArgs& GetTimeSliderArgs()
	{
		return TimeSliderArgs;
	}
};
