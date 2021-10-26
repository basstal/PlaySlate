#include "ActViewport.h"

#include "PlaySlate.h"
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
{
	TransportControlsState = NovaDB::Create("TransportControlsState", ENovaTransportControls::ForwardPlay);
	PreviewInstanceLooping = NovaDB::Create("PreviewInstanceLooping", false);
	PreviewInstancePlaybackMode = NovaDB::Create("PreviewInstancePlaybackMode", EPlaybackMode::Stopped);
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	CurrentTimeDB = NovaDB::CreateSP("ActEventTimelineArgs/CurrentTime", ActEventTimelineArgsDB->GetData()->CurrentTime);
}

FActViewport::~FActViewport()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActActionViewportWidget.Reset();
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	ActEventTimelineArgsDB->UnBind(OnCurrentTimeChangedHandle);
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	ActAnimationDB->UnBind(OnAnimBlueprintChangedHandle);
	ActAnimationDB->UnBind(OnAnimSequenceChangedHandle);
	NovaDB::Delete("TransportControlsState");
	NovaDB::Delete("PreviewInstanceLooping");
	NovaDB::Delete("PreviewInstancePlaybackMode");
	NovaDB::Delete("ActEventTimelineArgs/CurrentTime");
}

void FActViewport::Init(const TSharedRef<SDockTab>& InParentDockTab)
{
	// ** NOTE:合理不能用SNew的原因是在构造时会调用到FActActionPreviewSceneController::MakeViewportClient，需要先设置好ActActionViewportWidget指针
	InParentDockTab->SetContent(SAssignNew(ActActionViewportWidget, SActActionViewportWidget, SharedThis(this)));

	OnCurrentTimeChangedHandle = CurrentTimeDB->Bind(TDataBindingSP<FFrameTime>::DelegateType::CreateRaw(this, &FActViewport::OnCurrentTimeChanged));

	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	OnAnimBlueprintChangedHandle = ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &FActViewport::OnAnimBlueprintChanged));
	OnAnimSequenceChangedHandle = ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &FActViewport::OnAnimSequenceChanged));

	TransportControlsState->Bind(TDataBinding<ENovaTransportControls>::DelegateType::CreateRaw(this, &FActViewport::OnTransportControlsStateChanged));
	PreviewInstancePlaybackMode->Bind(TDataBinding<EPlaybackMode::Type>::DelegateType::CreateRaw(this, &FActViewport::OnPlaybackModeChanged));
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
	TickPlayingStopped();
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
	UE_LOG(LogNovaAct, Log, TEXT("SpawnActorInViewport ActActionActor : %s"), *ActActionActor->GetName());

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

void FActViewport::OnCurrentTimeChanged(TSharedPtr<FFrameTime> InCurrentTime)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		const FFrameRate TickResolution = DB->GetData()->TickResolution;
		float PreviewScrubTime = TickResolution.AsSeconds(*InCurrentTime);
		ActActionSkeletalMesh->PreviewInstance->SetPosition(PreviewScrubTime);
		ActActionSkeletalMesh->PreviewInstance->UpdateAnimation(PreviewScrubTime, false);
	}
}

void FActViewport::EvaluateToOneEnd(bool bIsEndEnd)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
		auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
		const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
		ActEventTimelineArgs->CurrentTime->FrameNumber = 0;
		if (bIsEndEnd)
		{
			*ActEventTimelineArgs->CurrentTime = TickResolution.AsFrameTime(ActEventTimelineArgs->ClampRange.GetUpperBoundValue());
		}
		CurrentTimeDB->Trigger();
	}
}

void FActViewport::OnPlaybackModeChanged(EPlaybackMode::Type InPlaybackMode) const
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

void FActViewport::ToggleLooping() const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		const bool bLoppingState = !PreviewInstanceLooping->GetData();
		PreviewInstance->SetLooping(bLoppingState);
		PreviewInstanceLooping->SetData(bLoppingState);
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
		PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
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
		auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
		const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
		// Advance a single frame, leaving it paused afterwards
		ActActionSkeletalMesh->GlobalAnimRateScale = 1.0f;
		ActActionSkeletalMesh->TickAnimation(TickResolution.AsInterval(), false);
		ActActionSkeletalMesh->GlobalAnimRateScale = 0.0f;
	}
}

// EPlaybackMode::Type FActViewport::GetPlaybackMode() const
// {
// 	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
// 	{
// 		if (PreviewInstance->IsPlaying())
// 		{
// 			return PreviewInstance->IsReverse() ? EPlaybackMode::PlayingReverse : EPlaybackMode::PlayingForward;
// 		}
// 		return EPlaybackMode::Stopped;
// 	}
// 	else if (ActActionSkeletalMesh)
// 	{
// 		return (ActActionSkeletalMesh->GlobalAnimRateScale > 0.0f) ? EPlaybackMode::PlayingForward : EPlaybackMode::Stopped;
// 	}
// 	return EPlaybackMode::Stopped;
// }

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
			auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
			*ActEventTimelineArgs->CurrentTime = ActEventTimelineArgs->TickResolution.AsFrameTime(CurrentTime);
			ActEventTimelineArgsDB->SetData(ActEventTimelineArgs);
		}
	}
}

void FActViewport::TickPlayingStopped()
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		if (!ActActionSkeletalMesh->PreviewInstance->IsLooping() && !ActActionSkeletalMesh->PreviewInstance->IsPlaying())
		{
			PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
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
		UE_LOG(LogNovaAct, Log, TEXT("FNovaActEditor::OnAnimBlueprintChanged with nullptr AnimBlueprint or AnimBlueprint->TargetSkeleton is nullptr"));
		return;
	}
	SpawnActorInViewport(ASkeletalMeshActor::StaticClass(), AnimBlueprint);
}

void FActViewport::OnAnimSequenceChanged(UActAnimation* InActAnimation)
{
	if (!InActAnimation)
	{
		return;
	}
	UAnimSequence* InAnimSequence = InActAnimation->AnimSequence;
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
				// ** 强制同步 PlaybackMode 状态，否则会因为 EnablePreview 调用而自动进入播放状态
				PreviewInstancePlaybackMode->Trigger();
				ActActionSkeletalMesh->PreviewInstance->SetLooping(true);
				PreviewInstanceLooping->SetData(true);
				ActActionSkeletalMesh->SetPlayRate(1.0f);

				//Place the camera at a good viewer position
				const TSharedPtr<FEditorViewportClient> ViewportClient = ActActionViewportWidget->GetViewportClient();
				FVector NewPosition = ViewportClient->GetViewLocation();
				NewPosition.Normalize();
				ViewportClient->SetViewLocation(NewPosition * (PreviewMesh->GetImportedBounds().SphereRadius * 1.5f));
			}
		}
	}
}

void FActViewport::OnTransportControlsStateChanged(ENovaTransportControls InNovaTransportControls)
{
	switch (InNovaTransportControls)
	{
	case ENovaTransportControls::ForwardStep:
		{
			PlayStep(true);
			break;
		}
	case ENovaTransportControls::BackwardStep:
		{
			PlayStep(false);
			break;
		}
	case ENovaTransportControls::ForwardEnd:
		{
			EvaluateToOneEnd(true);
			break;
		}
	case ENovaTransportControls::BackwardEnd:
		{
			EvaluateToOneEnd(false);
			break;
		}
	case ENovaTransportControls::ForwardPlay:
		{
			const EPlaybackMode::Type PlaybackMode = PreviewInstancePlaybackMode->GetData();
			if (PlaybackMode == EPlaybackMode::Stopped || PlaybackMode == EPlaybackMode::PlayingReverse)
			{
				PreviewInstancePlaybackMode->SetData(EPlaybackMode::PlayingForward);
			}
			else
			{
				PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
			}
			break;
		}
	case ENovaTransportControls::BackwardPlay:
		{
			const EPlaybackMode::Type PlaybackMode = PreviewInstancePlaybackMode->GetData();
			if (PlaybackMode == EPlaybackMode::Stopped || PlaybackMode == EPlaybackMode::PlayingForward)
			{
				PreviewInstancePlaybackMode->SetData(EPlaybackMode::PlayingReverse);
			}
			else
			{
				PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
			}
			break;
		}
	case ENovaTransportControls::ToggleLooping:
		{
			ToggleLooping();
			break;
		}
	case ENovaTransportControls::Pause:
		{
			PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
			break;
		}
	default: ;
	}
}
