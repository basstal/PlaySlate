#include "ActActionPreviewSceneController.h"

#include "ActActionViewportClient.h"
#include "PlaySlate.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"

#include "Animation/DebugSkelMeshComponent.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"

FActActionPreviewSceneController::FActActionPreviewSceneController(const ConstructionValues& CVS, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: FAdvancedPreviewScene(CVS),
	  ActActionActor(nullptr),
	  ActActionSkeletalMesh(nullptr),
	  LastCachedLODForPreviewComponent(0),
	  ActActionSequenceEditor(InActActionSequenceEditor)
{
}

FActActionPreviewSceneController::~FActActionPreviewSceneController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActActionViewportWidget.Reset();
}

void FActActionPreviewSceneController::InitPreviewScene(AActor* InActor)
{
	// setup default scene
	if (InActor)
	{
		UE_LOG(LogActAction, Log, TEXT("InitPreviewScene InActor : %s"), *InActor->GetName());
		check(!InActor || !InActor->IsRooted());
		ActActionActor = InActor;
	}
	else
	{
		InActor = GetWorld()->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity);
		check(!InActor || !InActor->IsRooted());
		ActActionActor = InActor;
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


void FActActionPreviewSceneController::SetPreviewMeshComponent(UDebugSkelMeshComponent* InSkeletalMeshComponent)
{
	ActActionSkeletalMesh = InSkeletalMeshComponent;

	if (ActActionSkeletalMesh)
	{
		ActActionSkeletalMesh->SelectionOverrideDelegate = UPrimitiveComponent::FSelectionOverride::CreateRaw(this, &FActActionPreviewSceneController::PreviewComponentSelectionOverride);
		ActActionSkeletalMesh->PushSelectionToProxy();
	}
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
