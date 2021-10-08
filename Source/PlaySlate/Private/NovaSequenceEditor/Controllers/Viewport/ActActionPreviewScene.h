#pragma once


#include "AdvancedPreviewScene.h"

class FActActionPreviewScene : public FAdvancedPreviewScene
{
public:
	FActActionPreviewScene(const ConstructionValues& CVS);
	virtual ~FActActionPreviewScene() override;
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
public:
	UDebugSkelMeshComponent* GetActActionSkeletalMesh() const
	{
		return ActActionSkeletalMesh;
	}
};
