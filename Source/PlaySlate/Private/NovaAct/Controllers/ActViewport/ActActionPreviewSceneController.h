#pragma once


#include "AdvancedPreviewScene.h"
#include "ITransportControl.h"

class FActActionViewportClient;

class SActActionViewportWidget;

class FActActionSequenceEditor;

struct FActActionHitBoxData;

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
	void SpawnActorInViewport(UClass* ActorType, const UAnimBlueprint* AnimBlueprint);

	/** @param InAnimSequence 初始化动画使用的Montage实例 */
	void InitAnimation(UAnimSequence* InAnimSequence) const;

	/**
	 * 根据EvaluationRange结构来设置当前动画的预览位置
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
	 * @param InPlaybackMode 当前播放状态
	 */
	void TogglePlay(const EPlaybackMode::Type& InPlaybackMode) const;

	/** 控制循环播放 */
	void ToggleLoop() const;

	/** @return 是否在循环播放 */
	bool IsLoopStatusOn() const;

	/**
	 * 逐帧播放
	 *
	 * @param bForward 是否是逐帧前进，否则倒退
	 */
	void PlayStep(bool bForward) const;

	/** @retrun 获得当前播放状态 */
	EPlaybackMode::Type GetPlaybackMode() const;

	/** @return 当前动画播放的时间位置，单位秒 */
	float GetCurrentPosition() const;

	/** @return 获得AnimInstance */
	UAnimSingleNodeInstance* GetAnimSingleNodeInstance() const;

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
