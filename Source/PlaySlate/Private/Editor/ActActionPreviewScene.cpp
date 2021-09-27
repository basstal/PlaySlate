#include "ActActionPreviewScene.h"
#include "PlaySlate/PlaySlate.h"

#include "Animation/DebugSkelMeshComponent.h"

FActActionPreviewScene::FActActionPreviewScene(const ConstructionValues& CVS)
	: FAdvancedPreviewScene(CVS)
	, ActActionActor(nullptr)
	, ActActionSkeletalMesh(nullptr)
{
	
}

void FActActionPreviewScene::InitPreviewScene(AActor * InActor)
{
	// setup default scene
	if (InActor)
	{
		UE_LOG(LogActAction, Log, TEXT("InitPreviewScene InActor : %s"), *InActor->GetName());
		SetPreviewActor(InActor);
	}
	else
	{
		SetPreviewActor(GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity));
	}

	// Create the preview component
	UDebugSkelMeshComponent* SkeletalMeshComponent = NewObject<UDebugSkelMeshComponent>(ActActionActor);
	if (GEditor->PreviewPlatform.GetEffectivePreviewFeatureLevel() <= ERHIFeatureLevel::ES3_1)
	{
		SkeletalMeshComponent->SetMobility(EComponentMobility::Static);
	}
	AddComponent(SkeletalMeshComponent, FTransform::Identity, false);
	SetPreviewMeshComponent(SkeletalMeshComponent);

	// set root component, so we can attach to it. 
	ActActionActor->SetRootComponent(SkeletalMeshComponent);
}

void FActActionPreviewScene::SetPreviewMeshComponent(UDebugSkelMeshComponent* InSkeletalMeshComponent)
{
	ActActionSkeletalMesh = InSkeletalMeshComponent; 

	if(ActActionSkeletalMesh)
	{
		ActActionSkeletalMesh->SelectionOverrideDelegate = UPrimitiveComponent::FSelectionOverride::CreateRaw(this, &FActActionPreviewScene::PreviewComponentSelectionOverride);
		ActActionSkeletalMesh->PushSelectionToProxy();	
	}
}

void FActActionPreviewScene::SetPreviewActor(AActor* InActor)
{
	check(!InActor || !InActor->IsRooted());
	ActActionActor = InActor;
}

void FActActionPreviewScene::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot)
{
	if (bAttachToRoot)
	{
		if (USceneComponent* SceneComponent = Cast<USceneComponent>(Component))
		{
			SceneComponent->AttachToComponent(ActActionActor->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		}
	}

	ActActionActor->AddOwnedComponent(Component);
	FAdvancedPreviewScene::AddComponent(Component, LocalToWorld, bAttachToRoot);
}

bool FActActionPreviewScene::PreviewComponentSelectionOverride(const UPrimitiveComponent* InComponent) const
{
	if (InComponent == GetActActionSkeletalMesh())
	{
		const USkeletalMeshComponent* Component = CastChecked<USkeletalMeshComponent>(InComponent);
		return (Component->GetSelectedEditorSection() != INDEX_NONE) || (Component->GetSelectedEditorMaterial() != INDEX_NONE);
	}

	return false;
}
