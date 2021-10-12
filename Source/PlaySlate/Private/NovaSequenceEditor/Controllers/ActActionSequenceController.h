#pragma once

#include "Utils/ActActionPlaybackUtil.h"
#include "Utils/ActActionSequenceUtil.h"

#include "IContentBrowserSingleton.h"

class FActActionSequenceEditor;
class FActActionTimeSliderController;
class SActActionSequenceWidget;
class ASkeletalMeshActor;
class UActActionSequence;
class FActActionSequenceTreeViewNode;
class SActActionViewportWidget;
/**
 * Sequence Tab的主要控制器
 * 对应的View模块为SActActionSequenceWidget
 */
class FActActionSequenceController : public TSharedFromThis<FActActionSequenceController>, FTickableEditorObject
{
public:
	FActActionSequenceController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionSequenceController() override;

	//~Begin FTickableEditorObject interface
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override;
	//~End FTickableEditorObject interface

	/**
	 * 调用已注册的TrackEditor的Create代理，并收集创建的TrackEditor实例
	 */
	void ExecuteTrackEditorCreateDelegate();
	/**
	 * TODO:
	 */
	void UpdateTimeBases();
	/**
	 * 构建AddTrack菜单的内容
	 *
	 * @param MenuBuilder 被修改的菜单构建者
	 */
	void BuildAddTrackMenu(FMenuBuilder& MenuBuilder);
	/**
	 * 添加AnimMontage的Track
	 *
	 * @param AnimMontage 被添加的AnimMontage实例
	 */
	void AddAnimMontageTrack(UAnimMontage* AnimMontage);
	/**
	 * 设置当前的播放状态
	 *
	 * @param InPlaybackStatus 设置为该播放状态
	 */
	void SetPlaybackStatus(ActActionSequence::EPlaybackType InPlaybackStatus);
	/**
	 * 根据EvaluationRange结构来获得当前动画的预览位置
	 *
	 * @param InRange 传入的Range
	 */
	void EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange);
	/**
	 * 将当前播放状态设置为Stopped
	 */
	void Pause();
	/**
	 * 构造Sequence的Widget为SActActionSequenceWidget，同时初始化TreeView相关内容
	 *
	 * @param ViewParams 构造Widget使用的相关参数
	 */
	void MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams);

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
	void SetGlobalTime(FFrameTime InFrameTime);
	/**
	 * 设置局部时间点到指定时间
	 * 
	 * @param InFrameTime 设置的时间点
	 */
	void SetLocalTimeDirectly(FFrameTime InFrameTime);
	/**
	 * 时间轴拖拽器位置改变的回调
	 */
	void OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing);
	/**
	 * 获得编辑器编辑的对象实例
	 */
	UActActionSequence* GetActActionSequence() const;
	/**
	 * 获得SequenceEditor，以便协同调用其他Controller
	 */
	TSharedRef<FActActionSequenceEditor> GetActActionSequenceEditor() const;
protected:
	/**
	 * 对Editor的弱引用，调用编辑器资源和相关工具方法
	 */
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
	/**
	 * 所有已注册的CreateTrackEditor代理方法，在FActActionTrackEditorBase的子类中实现
	 */
	TArray<ActActionSequence::OnCreateTrackEditorDelegate> TrackEditorDelegates;
	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;
	/**
	 * 当前的播放状态
	 */
	ActActionSequence::EPlaybackType PlaybackState;
	/**
	 * 当前的播放位置相关的数据结构
	 */
	ActActionSequence::FActActionPlaybackPosition PlayPosition;
	/**
	 * TimeSlider的Controller
	 */
	TSharedPtr<FActActionTimeSliderController> TimeSliderController;
	/**
	 * 所有可见节点DisplayedRootNodes的父节点，
	 * Sequence中所有可见根节点都储存在NodeTree中作为子节点
	 */
	TSharedPtr<FActActionSequenceTreeViewNode> TreeViewRoot;
	/**
	 * UMG Sequence main
	 */
	TSharedPtr<SActActionSequenceWidget> SequenceWidget;
	/** Numeric type interface used for converting parsing and generating strings from numbers */
	TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
public:
	ActActionSequence::EPlaybackType GetPlaybackStatus() const
	{
		return PlaybackState;
	}

	TSharedRef<SActActionSequenceWidget> GetSequenceWidget() const
	{
		return SequenceWidget.ToSharedRef();
	}

	TSharedRef<FActActionSequenceTreeViewNode> GetTreeViewRoot() const
	{
		return TreeViewRoot.ToSharedRef();
	}
	TSharedRef<FActActionTimeSliderController> GetTimeSliderController() const
	{
		return TimeSliderController.ToSharedRef();
	}
	TSharedRef<INumericTypeInterface<double>> GetNumericType() const
	{
		return NumericTypeInterface.ToSharedRef();
	}
};
