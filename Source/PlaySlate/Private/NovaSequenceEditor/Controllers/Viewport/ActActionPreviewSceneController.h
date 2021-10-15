#pragma once


#include "AdvancedPreviewScene.h"
#include "ITransportControl.h"
#include "Utils/ActActionPlaybackUtil.h"

class FActActionViewportClient;
class SActActionViewportWidget;
class FActActionSequenceEditor;

class FActActionPreviewSceneController : public TSharedFromThis<FActActionPreviewSceneController>, public FAdvancedPreviewScene
{
public:
	FActActionPreviewSceneController(const ConstructionValues& CVS, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionPreviewSceneController() override;

	/**
	* 构造Sequence的Widget为SActActionViewportWidget
	*/
	void MakeViewportWidget();
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
	void SpawnActorInViewport(UClass* ActorType, UAnimBlueprint* AnimBlueprint);
	/**
	 * 根据montage实例初始化角色当前动画
	 *
	 * @param AnimMontage 初始化动画使用的Montage实例
	 */
	void InitAnimationByAnimMontage(UAnimMontage* AnimMontage);
	/**
	 * 根据EvaluationRange结构来获得当前动画的预览位置
	 *
	 * @param InRange 传入的Range
	 */
	void EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange);
	/**
	 * 设置到动画的一端，开始或结尾
	 *
	 * @param bIsEndEnd 是结尾端
	 */
	void EvaluateToOneEnd(bool bIsEndEnd);
	/**
	 * 动画播放的点击事件，同时控制播放、暂停，以及播放的方向
	 *
	 * @param bForward 是前进播放，否则倒退播放
	 */
	void TogglePlay(bool bForward);
	/** 控制循环播放 */
	void ToggleLoop();
	/** @return 是否在循环播放 */
	bool IsLoopStatusOn();
	/**
	 * 逐帧播放
	 *
	 * @param bForward 是否是逐帧前进，否则倒退
	 */
	void PlayStep(bool bForward);
	/** @retrun 获得当前播放状态 */
	EPlaybackMode::Type GetPlaybackMode();
protected:
	/**
	* 对Editor的引用，调用编辑器资源和相关工具方法
	*/
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
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
	/**
	 * 当前预览动画的定格时间，单位秒
	 */
	double PreviewScrubTime;
public:
	TSharedRef<SActActionViewportWidget> GetActActionViewportWidget() const
	{
		return ActActionViewportWidget.ToSharedRef();
	}
};
