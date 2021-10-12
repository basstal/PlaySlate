#pragma once


#include "AdvancedPreviewScene.h"
#include "Utils/ActActionPlaybackUtil.h"

class FActActionViewportClient;
class SActActionViewportWidget;
class FActActionSequenceEditor;

class FActActionPreviewSceneController : public TSharedFromThis<FActActionPreviewSceneController>, public FAdvancedPreviewScene
{
public:
	FActActionPreviewSceneController(const ConstructionValues& CVS, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionPreviewSceneController() override;

	/** Override for preview component selection to inform the editor we consider it selected */
	bool PreviewComponentSelectionOverride(const UPrimitiveComponent* InComponent) const;
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

	//~Begin FPreviewScene interface
	virtual void AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot) override;
	//~End FPreviewScene interface
	
	//~Begin FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	//~End FTickableObjectBase interface

	TSharedPtr<FActActionViewportClient> MakeViewportClient(const TSharedRef<SActActionViewportWidget>& InViewportWidget);

	void MakeViewportWidget();
protected:
	/** The one and only actor we have */
	AActor* ActActionActor;

	/** The main preview skeletal mesh component */
	UDebugSkelMeshComponent* ActActionSkeletalMesh;

	/** Cached bounds of the floor mesh */
	FBoxSphereBounds FloorBounds;

	/** LOD index cached & used to check for broadcasting OnLODChanged delegate */
	int32 LastCachedLODForPreviewComponent;

	/** LOD changed delegate */
	FSimpleMulticastDelegate OnLODChanged;
	TSharedPtr<SActActionViewportWidget> ActActionViewportWidget;
	// TSharedPtr<FActActionViewportClient> ViewportClient;
	/**
	* 对Editor的引用，调用编辑器资源和相关工具方法
	*/
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
	/**
	 * 当前预览动画的定格位置
	 */
	double PreviewScrubTime;
public:
	TSharedRef<SActActionViewportWidget> GetActActionViewportWidget() const
	{
		return ActActionViewportWidget.ToSharedRef();
	}
	UDebugSkelMeshComponent* GetActActionSkeletalMesh() const
	{
		return ActActionSkeletalMesh;
	}
};
