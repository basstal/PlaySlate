#include "ActViewport.h"

#include "Common/NovaStaticFunction.h"
#include "PlaySlate.h"
#include "ActActionViewportClient.h"
#include "NovaAct/Widgets/ActViewport/ActActionViewportWidget.h"
#include "NovaAct/NovaActEditor.h"

#include "Animation/DebugSkelMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "AnimPreviewInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
#include "NovaAct/Assets/ActAnimation.h"

FActViewport::FActViewport(const ConstructionValues& CVS, const TSharedRef<FNovaActEditor>& InActActionSequenceEditor)
	: FAdvancedPreviewScene(CVS),
	  ActActionSequenceEditor(InActActionSequenceEditor),
	  ActActionActor(nullptr),
	  ActActionSkeletalMesh(nullptr),
	  LastCurrentTime(0)
{ }

FActViewport::~FActViewport()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActActionViewportWidget.Reset();
	auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	ActEventTimelineArgsDB->UnBind(OnCurrentTimeChangedHandle);
	auto ActAnimationDB = NovaDB::GetOrCreate<UActAnimation*>("ActAnimation");
	ActAnimationDB->UnBind(OnAnimBlueprintChangedHandle);
}

void FActViewport::Init()
{
	// ** NOTE:合理不能用SNew的原因是在构造时会调用到FActActionPreviewSceneController::MakeViewportClient，需要先设置好ActActionViewportWidget指针
	SAssignNew(ActActionViewportWidget, SActActionViewportWidget, SharedThis(this));

	auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	OnCurrentTimeChangedHandle = ActEventTimelineArgsDB->Bind(CreateRawBind(this, &FActViewport::EvaluateInternal, TSharedPtr<FActEventTimelineArgs>));

	auto ActAnimationDB = NovaDB::GetOrCreate<UActAnimation*>("ActAnimation");
	OnAnimBlueprintChangedHandle = ActAnimationDB->Bind(CreateRawBind(this, &FActViewport::OnAnimBlueprintChanged, UActAnimation*));
}

TSharedPtr<FActActionViewportClient> FActViewport::MakeViewportClient()
{
	return MakeShareable(new FActActionViewportClient(
		SharedThis(this),
		ActActionViewportWidget.ToSharedRef(),
		ActActionSequenceEditor.Pin()->GetEditorModeManager()));
}

void FActViewport::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot)
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

void FActViewport::Tick(float DeltaTime)
{
	FAdvancedPreviewScene::Tick(DeltaTime);
	if (!GIntraFrameDebuggingGameThread)
	{
		GetWorld()->Tick(LEVELTICK_All, DeltaTime);
	}
	if (ActActionSkeletalMesh)
	{
		// Handle updating the preview component to represent the effects of root motion
		ActActionSkeletalMesh->ConsumeRootMotion(FloorBounds.GetBox().Min, FloorBounds.GetBox().Max);
	}
	TickCurrentTimeChanged();
}

void FActViewport::SpawnActorInViewport(UClass* ActorType, const UAnimBlueprint* AnimBlueprint)
{
	if (ActActionActor)
	{
		RemoveComponent(ActActionSkeletalMesh);
		ActActionActor->Destroy();
	}
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
	const float BoundsZOffset = NovaStaticFunction::GetBoundsZOffset(ActActionSkeletalMesh->Bounds);
	ActActionActor->SetActorLocation(FVector(0, 0, BoundsZOffset) - ActActionSkeletalMesh->Bounds.Origin, false);
	ActActionSkeletalMesh->RecreateRenderState_Concurrent();
}

void FActViewport::InitAnimation(UAnimSequence* InAnimSequence) const
{
	USkeleton* Skeleton = nullptr;
	if (InAnimSequence)
	{
		Skeleton = InAnimSequence->GetSkeleton();
	}
	if (ActActionSkeletalMesh && Skeleton)
	{
		USkeletalMesh* PreviewMesh = Skeleton->GetAssetPreviewMesh(InAnimSequence);
		if (PreviewMesh)
		{
			UAnimSingleNodeInstance* PreviewInstance = Cast<UAnimSingleNodeInstance>(ActActionSkeletalMesh->PreviewInstance);
			if (!PreviewInstance || PreviewInstance->GetCurrentAsset() != InAnimSequence || ActActionSkeletalMesh->SkeletalMesh != PreviewMesh)
			{
				ActActionSkeletalMesh->SetSkeletalMeshWithoutResettingAnimation(PreviewMesh);
				ActActionSkeletalMesh->EnablePreview(true, InAnimSequence);
				ActActionSkeletalMesh->PreviewInstance->SetLooping(true);
				ActActionSkeletalMesh->SetPlayRate(2.0f);

				//Place the camera at a good viewer position
				const TSharedPtr<FEditorViewportClient> ViewportClient = ActActionViewportWidget->GetViewportClient();
				FVector NewPosition = ViewportClient->GetViewLocation();
				NewPosition.Normalize();
				ViewportClient->SetViewLocation(NewPosition * (PreviewMesh->GetImportedBounds().SphereRadius * 1.5f));
			}
		}
	}
}

void FActViewport::EvaluateInternal(TSharedPtr<FActEventTimelineArgs> InArgs)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		if (ActActionSkeletalMesh->PreviewInstance->IsPlaying())
		{
			ActActionSkeletalMesh->PreviewInstance->SetPlaying(false);
		}

		const FFrameTime CurrentPosition = InArgs->CurrentTime;
		const FFrameRate TickResolution = InArgs->TickResolution;
		float PreviewScrubTime = TickResolution.AsSeconds(CurrentPosition);
		ActActionSkeletalMesh->PreviewInstance->SetPosition(PreviewScrubTime);
		ActActionSkeletalMesh->PreviewInstance->UpdateAnimation(PreviewScrubTime, false);
	}
}

void FActViewport::EvaluateToOneEnd(bool bIsEndEnd)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		auto DB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
		const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
		ActEventTimelineArgs->CurrentTime = FFrameTime(0);
		if (bIsEndEnd)
		{
			ActEventTimelineArgs->CurrentTime = TickResolution.AsFrameTime(ActEventTimelineArgs->ClampRange.GetUpperBoundValue());
		}
		DB->Trigger();
	}
}

void FActViewport::TogglePlay(const EPlaybackMode::Type& InPlaybackMode) const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		const bool bPlay = InPlaybackMode != EPlaybackMode::Stopped;
		const bool bReverse = InPlaybackMode == EPlaybackMode::PlayingReverse;
		const bool bIsReverse = PreviewInstance->IsReverse();
		const bool bIsPlaying = PreviewInstance->IsPlaying();
		if (bPlay != bIsPlaying)
		{
			// 播放状态与待设置的播放状态不同，改变播放状态
			if (PreviewInstance->GetCurrentTime() >= PreviewInstance->GetLength())
			{
				PreviewInstance->SetPosition(0.0f, false);
			}
			PreviewInstance->SetPlaying(bPlay);
			PreviewInstance->SetReverse(bReverse);
			if (ActActionSkeletalMesh && ActActionSkeletalMesh->bPauseClothingSimulationWithAnim)
			{
				if (bPlay)
				{
					ActActionSkeletalMesh->ResumeClothingSimulation();
				}
				else
				{
					ActActionSkeletalMesh->SuspendClothingSimulation();
				}
			}
		}
		else if (bPlay && bReverse != bIsReverse)
		{
			// 如果当前正在播放，并且播放顺序与传入参数相反，则反置Reverse参数
			PreviewInstance->SetReverse(bReverse);
		}
	}
	else if (ActActionSkeletalMesh)
	{
		ActActionSkeletalMesh->GlobalAnimRateScale = (ActActionSkeletalMesh->GlobalAnimRateScale > 0.0f) ? 0.0f : 1.0f;
	}
}

void FActViewport::ToggleLoop() const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		const bool bIsLooping = PreviewInstance->IsLooping();
		PreviewInstance->SetLooping(!bIsLooping);
	}
}

bool FActViewport::IsLoopStatusOn() const
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->PreviewInstance)
	{
		return ActActionSkeletalMesh->PreviewInstance->IsLooping();
	}
	return false;
}

void FActViewport::PlayStep(bool bForward) const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		const bool bShouldStepCloth = FMath::Abs(PreviewInstance->GetLength() - PreviewInstance->GetCurrentTime()) > SMALL_NUMBER;
		PreviewInstance->SetPlaying(false);
		if (bForward)
		{
			PreviewInstance->StepForward();
		}
		else
		{
			PreviewInstance->StepBackward();
		}
		if (ActActionSkeletalMesh && bShouldStepCloth)
		{
			ActActionSkeletalMesh->bPerformSingleClothingTick = true;
		}
	}
	else if (ActActionSkeletalMesh && bForward)
	{
		auto DB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
		const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
		// Advance a single frame, leaving it paused afterwards
		ActActionSkeletalMesh->GlobalAnimRateScale = 1.0f;
		ActActionSkeletalMesh->TickAnimation(TickResolution.AsInterval(), false);
		ActActionSkeletalMesh->GlobalAnimRateScale = 0.0f;
	}
}

EPlaybackMode::Type FActViewport::GetPlaybackMode() const
{
	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		if (PreviewInstance->IsPlaying())
		{
			return PreviewInstance->IsReverse() ? EPlaybackMode::PlayingReverse : EPlaybackMode::PlayingForward;
		}
		return EPlaybackMode::Stopped;
	}
	else if (ActActionSkeletalMesh)
	{
		return (ActActionSkeletalMesh->GlobalAnimRateScale > 0.0f) ? EPlaybackMode::PlayingForward : EPlaybackMode::Stopped;
	}
	return EPlaybackMode::Stopped;
}

float FActViewport::GetCurrentPosition() const
{
	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		return PreviewInstance->GetCurrentTime();
	}
	return 0.0f;
}

UAnimSingleNodeInstance* FActViewport::GetAnimSingleNodeInstance() const
{
	if (ActActionSkeletalMesh)
	{
		return Cast<UAnimSingleNodeInstance>(ActActionSkeletalMesh->PreviewInstance);
	}
	return nullptr;
}

void FActViewport::TickCurrentTimeChanged()
{
	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		float CurrentTime = PreviewInstance->GetCurrentTime();
		if (CurrentTime != LastCurrentTime)
		{
			LastCurrentTime = CurrentTime;
			auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
			ActEventTimelineArgs->CurrentTime = ActEventTimelineArgs->TickResolution.AsFrameTime(CurrentTime);
			ActEventTimelineArgsDB->SetData(ActEventTimelineArgs);
		}
	}
}

void FActViewport::OnAnimBlueprintChanged(UActAnimation* InActAnimation)
{
	if (!InActAnimation)
	{
		return;
	}
	UAnimBlueprint* AnimBlueprint = InActAnimation->AnimBlueprint;
	if (!AnimBlueprint || !AnimBlueprint->TargetSkeleton)
	{
		UE_LOG(LogActAction, Log, TEXT("FNovaActEditor::OnAnimBlueprintChanged with nullptr AnimBlueprint or AnimBlueprint->TargetSkeleton is nullptr"));
		return;
	}
	SpawnActorInViewport(ASkeletalMeshActor::StaticClass(), AnimBlueprint);
}
