#pragma once


#include "AdvancedPreviewScene.h"
#include "ITransportControl.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"
#include "NovaAct/Assets/ActAnimation.h"

class FActActionViewportClient;
class SActActionViewportWidget;
class FNovaActEditor;
struct FActActionHitBoxData;

using namespace NovaStruct;

class FActViewport : public TSharedFromThis<FActViewport>, public FAdvancedPreviewScene
{
public:
	FActViewport(const ConstructionValues& CVS, const TSharedRef<FNovaActEditor>& InActActionSequenceEditor);
	virtual ~FActViewport() override;

	/**
	* @param InParentDockTab 子Widget所附着的DockTab
	*/
	void Init(const TSharedRef<SDockTab>& InParentDockTab);

	/**
	* Widget Make Client回调方法
	* 
	* @return 构造的FActActionViewportClient，是SEditorViewport必须实现的组件之一
	*/
	TSharedPtr<FActActionViewportClient> MakeViewportClient();

	//~Begin FPreviewScene interface
	virtual void AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot) override;
	//~End FPreviewScene interface

	//~Begin FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	//~End FTickableObjectBase interface

	/**
	 * 在Viewport中生成指定ActorType类型的Actor
	 *
	 * @param ActorType 指定的Actor类型
	 * @param AnimBlueprint 生成Actor指定的AnimBlueprint资源
	 */
	void SpawnActorInViewport(UClass* ActorType, const UAnimBlueprint* AnimBlueprint);

	/**
	 * 根据EvaluationRange结构来设置当前动画的预览位置
	 *
	 * @param InCurrentTime
	 */
	void OnCurrentTimeChanged(TSharedPtr<FFrameTime> InCurrentTime);

	/**
	 * 设置到动画的一端，开始或结尾
	 *
	 * @param bIsEndEnd 是结尾端
	 */
	void EvaluateToOneEnd(bool bIsEndEnd);

	/**
	 * 动画播放的点击事件，同时控制播放、暂停，以及播放的方向
	 *
	 * @param InPlaybackMode 当前播放状态
	 */
	void OnPlaybackModeChanged(EPlaybackMode::Type InPlaybackMode) const;

	/** 控制循环播放 */
	void ToggleLooping() const;

	/** @return 是否在循环播放 */
	bool IsLoopStatusOn() const;

	/**
	 * 逐帧播放
	 *
	 * @param bForward 是否是逐帧前进，否则倒退
	 */
	void PlayStep(bool bForward) const;

	/** @retrun 获得当前播放状态 */
	// EPlaybackMode::Type GetPlaybackMode() const;

	/** @return 当前动画播放的时间位置，单位秒 */
	float GetCurrentPosition() const;

	/** @return 获得AnimInstance */
	UAnimSingleNodeInstance* GetAnimSingleNodeInstance() const;

	/** 监听 AnimInstance 的 CurrentTime 是否已修改 */
	void TickCurrentTimeChanged();
	/** 监听 AnimSequence 的播放状态是否已停止*/
	void TickPlayingStopped();
	/** UActAnimation 的数据绑定，监听AnimBlueprint数据改变 */
	void OnAnimBlueprintChanged(UActAnimation* InActAnimation);
	/** UActAnimation 的数据绑定，监听AnimSequence数据改变 */
	void OnAnimSequenceChanged(UActAnimation* InActAnimation);
	/** ENovaTransportControls 的数据绑定，监听ENovaTransportControls数据改变，控制Viewport的动画播放 */
	void OnTransportControlsStateChanged(ENovaTransportControls InNovaTransportControls);


protected:
	/**
	* 对Editor的引用，调用编辑器资源和相关工具方法
	*/
	TWeakPtr<FNovaActEditor> ActActionSequenceEditor;

	/** The one and only actor we have */
	AActor* ActActionActor;

	/** The main preview skeletal mesh component */
	UDebugSkelMeshComponent* ActActionSkeletalMesh;

	/** Cached bounds of the floor mesh */
	FBoxSphereBounds FloorBounds;

	/**
	 * PreviewScene Main Widget
	 */
	TSharedPtr<SActActionViewportWidget> ActActionViewportWidget;

	// /**
	//  * 当前预览动画的定格时间，单位秒
	//  */
	// double PreviewScrubTime;

	float LastCurrentTime;                       // ** 用于检测CurrentTime是否改变
	FDelegateHandle OnCurrentTimeChangedHandle;  // ** 数据绑定
	FDelegateHandle OnAnimBlueprintChangedHandle;// ** 数据绑定
	FDelegateHandle OnAnimSequenceChangedHandle;


	TSharedPtr<TDataBinding<ENovaTransportControls>> TransportControlsState;  // ** 当前Viewport transport controls 状态的数据绑定
	TSharedPtr<TDataBinding<bool>> PreviewInstanceLooping;                    // ** 当前 Viewport 动画实例播放是否为 Lopping 状态
	TSharedPtr<TDataBinding<EPlaybackMode::Type>> PreviewInstancePlaybackMode;//** 当前 Viewport 动画实例播放 PlaybackMode 状态
	TSharedPtr<TDataBindingSP<FFrameTime>> CurrentTimeDB;                     // ** 当前 Viewport 动画实例的实际时间点
// public:
// 	TSharedRef<SActActionViewportWidget> GetActActionViewportWidget() const
// 	{
// 		return ActActionViewportWidget.ToSharedRef();
// 	}
};
