#pragma once


#include "AdvancedPreviewScene.h"

class FActActionPreviewScene : public FAdvancedPreviewScene
{
public:
	FActActionPreviewScene(const ConstructionValues& CVS);
	/**
	 * 构造角色和相关组件
	 */
	void InitPreviewScene(AActor* InActor);

	void SetPreviewMeshComponent(UDebugSkelMeshComponent* InSkeletalMeshComponent);

	/** Set the main actor */
	void SetPreviewActor(AActor* InActor);

	/** Override for preview component selection to inform the editor we consider it selected */
	bool PreviewComponentSelectionOverride(const UPrimitiveComponent* InComponent) const;

	//~Begin FPreviewScene interface
	virtual void AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot) override;
	//~End FPreviewScene interface
protected:
	/** The one and only actor we have */
	AActor* ActActionActor;

	/** The main preview skeletal mesh component */
	UDebugSkelMeshComponent* ActActionSkeletalMesh;
public:
	UDebugSkelMeshComponent* GetActActionSkeletalMesh() const
	{
		return ActActionSkeletalMesh;
	}
};
