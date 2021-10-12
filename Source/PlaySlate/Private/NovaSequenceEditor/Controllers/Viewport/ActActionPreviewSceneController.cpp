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
	  ActActionActor(nullptr),
	  ActActionSkeletalMesh(nullptr),
	  LastCachedLODForPreviewComponent(0),
	  ActActionSequenceEditor(InActActionSequenceEditor),
	  PreviewScrubTime(0)
{
}

FActActionPreviewSceneController::~FActActionPreviewSceneController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActActionViewportWidget.Reset();
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
	// OnPreTickDelegate.Broadcast();

	FAdvancedPreviewScene::Tick(DeltaTime);
	if (!GIntraFrameDebuggingGameThread)
	{
		GetWorld()->Tick(LEVELTICK_All, DeltaTime);
	}

	// Handle updating the preview component to represent the effects of root motion	
	ActActionSkeletalMesh->ConsumeRootMotion(FloorBounds.GetBox().Min, FloorBounds.GetBox().Max);

	if (LastCachedLODForPreviewComponent != ActActionSkeletalMesh->PredictedLODLevel)
	{
		OnLODChanged.Broadcast();
		LastCachedLODForPreviewComponent = ActActionSkeletalMesh->PredictedLODLevel;
	}

	// OnPostTickDelegate.Broadcast();
}

TSharedPtr<FActActionViewportClient> FActActionPreviewSceneController::MakeViewportClient(const TSharedRef<SActActionViewportWidget>& InViewportWidget)
{
	return MakeShareable(new FActActionViewportClient(
		this->AsShared(),
		InViewportWidget,
		ActActionSequenceEditor.Pin().ToSharedRef()));
}

void FActActionPreviewSceneController::MakeViewportWidget()
{
	SAssignNew(ActActionViewportWidget, SActActionViewportWidget, ActActionSequenceEditor.Pin().ToSharedRef());
}

bool FActActionPreviewSceneController::PreviewComponentSelectionOverride(const UPrimitiveComponent* InComponent) const
{
	if (InComponent == GetActActionSkeletalMesh())
	{
		const USkeletalMeshComponent* Component = CastChecked<USkeletalMeshComponent>(InComponent);
		return (Component->GetSelectedEditorSection() != INDEX_NONE) || (Component->GetSelectedEditorMaterial() != INDEX_NONE);
	}

	return false;
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

	ActActionSkeletalMesh->SelectionOverrideDelegate = UPrimitiveComponent::FSelectionOverride::CreateRaw(this, &FActActionPreviewSceneController::PreviewComponentSelectionOverride);
	ActActionSkeletalMesh->PushSelectionToProxy();
	ActActionSkeletalMesh->OverrideMaterials.Empty();

	USkeletalMesh* PreviewSkeletalMesh = AnimBlueprint->TargetSkeleton->GetPreviewMesh(true);
	ActActionSkeletalMesh->SetSkeletalMesh(PreviewSkeletalMesh);
	ActActionSkeletalMesh->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
	ActActionSkeletalMesh->InitializeAnimScriptInstance();
	ActActionSkeletalMesh->UpdateBounds();

	// Center the mesh at the world origin then offset to put it on top of the plane
	const float BoundsZOffset = ActActionSequence::ActActionStaticUtil::GetBoundsZOffset(ActActionSkeletalMesh->Bounds);
	ActActionActor->SetActorLocation(-ActActionSkeletalMesh->Bounds.Origin + FVector(0, 0, BoundsZOffset), false);
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

	UDebugSkelMeshComponent* PreviewComponent = GetActActionSkeletalMesh();
	if (PreviewComponent && Skeleton)
	{
		USkeletalMesh* PreviewMesh = Skeleton->GetAssetPreviewMesh(AnimSequence);
		if (PreviewMesh)
		{
			UAnimSingleNodeInstance* PreviewInstance = Cast<UAnimSingleNodeInstance>(PreviewComponent->PreviewInstance);
			if (!PreviewInstance || PreviewInstance->GetCurrentAsset() != AnimSequence || PreviewComponent->SkeletalMesh != PreviewMesh)
			{
				// PreviewInstance->CurrentAsset = AnimSequence;
				// PreviewInstance->CurrentSkeleton = PreviewMesh->GetSkeleton();
				// PreviewInstance->Montage_Play(AnimMontage);
				PreviewComponent->SetSkeletalMeshWithoutResettingAnimation(PreviewMesh);
				PreviewComponent->EnablePreview(true, AnimSequence);
				PreviewComponent->PreviewInstance->SetLooping(true);
				PreviewComponent->SetPlayRate(60);

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
	UDebugSkelMeshComponent* PreviewMeshComponent = GetActActionSkeletalMesh();
	if (PreviewMeshComponent && PreviewMeshComponent->IsPreviewOn())
	{
		if (PreviewMeshComponent->PreviewInstance->IsPlaying())
		{
			PreviewMeshComponent->PreviewInstance->SetPlaying(false);
		}
		FFrameTime CurrentPosition = InRange.GetTime();
		PreviewScrubTime = CurrentPosition.AsDecimal() / 60;
		PreviewMeshComponent->PreviewInstance->SetPosition(PreviewScrubTime);
		PreviewMeshComponent->PreviewInstance->UpdateAnimation(PreviewScrubTime, false);
	}
}
