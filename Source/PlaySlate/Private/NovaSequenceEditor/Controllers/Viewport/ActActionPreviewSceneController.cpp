#include "ActActionPreviewSceneController.h"

#include "Utils/ActActionStaticUtil.h"
#include "Utils/ActActionPlaybackUtil.h"
#include "PlaySlate.h"
#include "ActActionViewportClient.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"

#include "Animation/DebugSkelMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "AnimPreviewInstance.h"

FActActionPreviewSceneController::FActActionPreviewSceneController(const ConstructionValues& CVS, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: FAdvancedPreviewScene(CVS),
	  ActActionSequenceEditor(InActActionSequenceEditor),
	  ActActionActor(nullptr),
	  ActActionSkeletalMesh(nullptr),
	  PreviewScrubTime(0)
{
}

FActActionPreviewSceneController::~FActActionPreviewSceneController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActActionViewportWidget.Reset();
}

void FActActionPreviewSceneController::MakeViewportWidget()
{
	// ** NOTE:合理不能用SNew的原因是在构造时会调用到FActActionPreviewSceneController::MakeViewportClient，需要先设置好ActActionViewportWidget指针
	SAssignNew(ActActionViewportWidget, SActActionViewportWidget, SharedThis(this));
}

TSharedPtr<FActActionViewportClient> FActActionPreviewSceneController::MakeViewportClient()
{
	return MakeShareable(new FActActionViewportClient(
		this->AsShared(),
		ActActionViewportWidget.ToSharedRef(),
		ActActionSequenceEditor.Pin()->GetEditorModeManager()));
}

void FActActionPreviewSceneController::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot)
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

void FActActionPreviewSceneController::Tick(float DeltaTime)
{
	FAdvancedPreviewScene::Tick(DeltaTime);
	if (!GIntraFrameDebuggingGameThread)
	{
		GetWorld()->Tick(LEVELTICK_All, DeltaTime);
	}
	// Handle updating the preview component to represent the effects of root motion
	ActActionSkeletalMesh->ConsumeRootMotion(FloorBounds.GetBox().Min, FloorBounds.GetBox().Max);
}

void FActActionPreviewSceneController::SpawnActorInViewport(UClass* ActorType, UAnimBlueprint* AnimBlueprint)
{
	AActor* SpawnedActor = GetWorld()->SpawnActor(ActorType);
	check(SpawnedActor)
	ActActionActor = SpawnedActor;
	UE_LOG(LogActAction, Log, TEXT("SpawnActorInViewport ActActionActor : %s"), *ActActionActor->GetName());

	// Create the preview component
	UDebugSkelMeshComponent* SkeletalMeshComponent = NewObject<UDebugSkelMeshComponent>(ActActionActor);
	ActActionSkeletalMesh = SkeletalMeshComponent;
	if (GEditor->PreviewPlatform.GetEffectivePreviewFeatureLevel() <= ERHIFeatureLevel::ES3_1)
	{
		ActActionSkeletalMesh->SetMobility(EComponentMobility::Static);
	}
	AddComponent(ActActionSkeletalMesh, FTransform::Identity, false);
	// set root component, so we can attach to it. 
	ActActionActor->SetRootComponent(ActActionSkeletalMesh);

	ActActionSkeletalMesh->OverrideMaterials.Empty();
	USkeletalMesh* PreviewSkeletalMesh = AnimBlueprint->TargetSkeleton->GetPreviewMesh(true);
	ActActionSkeletalMesh->SetSkeletalMesh(PreviewSkeletalMesh);
	ActActionSkeletalMesh->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
	ActActionSkeletalMesh->InitializeAnimScriptInstance();
	ActActionSkeletalMesh->UpdateBounds();

	// Center the mesh at the world origin then offset to put it on top of the plane
	const float BoundsZOffset = ActActionSequence::ActActionStaticUtil::GetBoundsZOffset(ActActionSkeletalMesh->Bounds);
	ActActionActor->SetActorLocation(FVector(0, 0, BoundsZOffset) - ActActionSkeletalMesh->Bounds.Origin, false);
	ActActionSkeletalMesh->RecreateRenderState_Concurrent();
}

void FActActionPreviewSceneController::InitAnimationByAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(AnimMontage);
	USkeleton* Skeleton = nullptr;
	if (AnimSequence)
	{
		Skeleton = AnimSequence->GetSkeleton();
	}
	if (ActActionSkeletalMesh && Skeleton)
	{
		USkeletalMesh* PreviewMesh = Skeleton->GetAssetPreviewMesh(AnimSequence);
		if (PreviewMesh)
		{
			UAnimSingleNodeInstance* PreviewInstance = Cast<UAnimSingleNodeInstance>(ActActionSkeletalMesh->PreviewInstance);
			if (!PreviewInstance || PreviewInstance->GetCurrentAsset() != AnimSequence || ActActionSkeletalMesh->SkeletalMesh != PreviewMesh)
			{
				ActActionSkeletalMesh->SetSkeletalMeshWithoutResettingAnimation(PreviewMesh);
				ActActionSkeletalMesh->EnablePreview(true, AnimSequence);
				ActActionSkeletalMesh->PreviewInstance->SetLooping(true);
				ActActionSkeletalMesh->SetPlayRate(60);

				//Place the camera at a good viewer position
				TSharedPtr<FEditorViewportClient> ViewportClient = ActActionViewportWidget->GetViewportClient();
				FVector NewPosition = ViewportClient->GetViewLocation();
				NewPosition.Normalize();
				ViewportClient->SetViewLocation(NewPosition * (PreviewMesh->GetImportedBounds().SphereRadius * 1.5f));
			}
		}
	}
}

void FActActionPreviewSceneController::EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		if (ActActionSkeletalMesh->PreviewInstance->IsPlaying())
		{
			ActActionSkeletalMesh->PreviewInstance->SetPlaying(false);
		}
		FFrameTime CurrentPosition = InRange.GetTime();
		PreviewScrubTime = CurrentPosition.AsDecimal() / 60;
		ActActionSkeletalMesh->PreviewInstance->SetPosition(PreviewScrubTime);
		ActActionSkeletalMesh->PreviewInstance->UpdateAnimation(PreviewScrubTime, false);
	}
}
