#pragma once


#include "AdvancedPreviewScene.h"

class FActActionViewportClient;
class SActActionViewportWidget;
class FActActionSequenceEditor;

class FActActionPreviewSceneController : public TSharedFromThis<FActActionPreviewSceneController>, public FAdvancedPreviewScene
{
public:
	FActActionPreviewSceneController(const ConstructionValues& CVS, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionPreviewSceneController() override;
	/**
	 * 构造角色和相关组件
	 */
	void InitPreviewScene(AActor* InActor);

	void SetPreviewMeshComponent(UDebugSkelMeshComponent* InSkeletalMeshComponent);

	/** Override for preview component selection to inform the editor we consider it selected */
	bool PreviewComponentSelectionOverride(const UPrimitiveComponent* InComponent) const;

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
public:
	TSharedPtr<SActActionViewportWidget> GetActActionViewportWidget() const
	{
		return ActActionViewportWidget;
	}
	UDebugSkelMeshComponent* GetActActionSkeletalMesh() const
	{
		return ActActionSkeletalMesh;
	}
};
