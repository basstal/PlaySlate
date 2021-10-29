﻿#include "ActViewportPreviewScene.h"

#include "PlaySlate.h"
#include "Common/NovaStaticFunction.h"
#include "ActViewportClient.h"
#include "NovaAct/ActViewport/ActViewport.h"
#include "NovaAct/NovaActEditor.h"

#include "Animation/DebugSkelMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "AnimPreviewInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "Common/NovaConst.h"

#include "NovaAct/Assets/ActAnimation.h"

FActViewportPreviewScene::FActViewportPreviewScene(const ConstructionValues& CVS)
	: FAdvancedPreviewScene(CVS),
	  ActActionActor(nullptr),
	  ActActionSkeletalMesh(nullptr),
	  LastCurrentTime(0)
{
	NovaDB::Create("TransportControlsState", ENovaTransportControls::ForwardPlay);
	PreviewInstanceLooping = NovaDB::Create("PreviewInstanceLooping", false);
	PreviewInstancePlaybackMode = NovaDB::Create("PreviewInstancePlaybackMode", EPlaybackMode::Stopped);
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	if (!ActEventTimelineArgsDB)
	{
		auto ActEventTimelineArgs = NovaStaticFunction::MakeActEventTimelineArgs();
		ActEventTimelineArgsDB = NovaDB::CreateSP("ActEventTimelineArgs", ActEventTimelineArgs);
	}
	NovaDB::CreateSP("ActEventTimelineArgs/CurrentTime", ActEventTimelineArgsDB->GetData()->CurrentTime);
	DataBindingSPBindRaw(FFrameTime, "ActEventTimelineArgs/CurrentTime", this, &FActViewportPreviewScene::OnCurrentTimeChanged, OnCurrentTimeChangedHandle);

	// auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	DataBindingUObjectBindRaw(UActAnimation, "ActAnimation", this, &FActViewportPreviewScene::OnAnimBlueprintChanged, OnAnimBlueprintChangedHandle);
	DataBindingUObjectBindRaw(UActAnimation, "ActAnimation", this, &FActViewportPreviewScene::OnAnimSequenceChanged, OnAnimSequenceChangedHandle);

	FDelegateHandle _;
	DataBindingBindRaw(ENovaTransportControls, "TransportControlsState", this, &FActViewportPreviewScene::OnTransportControlsStateChanged, _);
	DataBindingBindRaw(EPlaybackMode::Type, "PreviewInstancePlaybackMode", this, &FActViewportPreviewScene::OnPlaybackModeChanged, _);
}

FActViewportPreviewScene::~FActViewportPreviewScene()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActActionPreviewSceneController::~FActActionPreviewSceneController"));
	ActViewport.Reset();
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs/CurrentTime");
	DB->UnBind(OnCurrentTimeChangedHandle);
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	if (ActAnimationDB)
	{
		ActAnimationDB->UnBind(OnAnimBlueprintChangedHandle);
		ActAnimationDB->UnBind(OnAnimSequenceChangedHandle);
	}
	NovaDB::Delete("TransportControlsState");
	NovaDB::Delete("PreviewInstanceLooping");
	NovaDB::Delete("PreviewInstancePlaybackMode");
	NovaDB::Delete("ActEventTimelineArgs/CurrentTime");
}

void FActViewportPreviewScene::Init(const TSharedRef<SDockTab>& InParentDockTab)
{
	// ** NOTE:这里不能用SNew的原因是在构造时会调用到FActActionPreviewSceneController::MakeViewportClient，需要先设置好ActActionViewportWidget指针
	// InParentDockTab->SetContent(SAssignNew(ActViewport, SActViewport, SharedThis(this)));
}

//
// TSharedPtr<FActViewportClient> FActViewportPreviewScene::MakeViewportClient()
// {
// 	
// }

void FActViewportPreviewScene::AddComponent(UActorComponent* Component, const FTransform& LocalToWorld, bool bAttachToRoot)
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

void FActViewportPreviewScene::Tick(float DeltaTime)
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
	// UE_LOG(LogNovaAct, Log, TEXT("FActViewportPreviewScene::Tick %f"), DeltaTime);
}

bool FActViewportPreviewScene::IsTickable() const
{
	// The preview scene is tickable if any viewport can see it
	return ActViewport.IsValid() && ActViewport->IsVisible();
}

ETickableTickType FActViewportPreviewScene::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

void FActViewportPreviewScene::SpawnActorInViewport(UClass* ActorType, const UAnimBlueprint* AnimBlueprint)
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

void FActViewportPreviewScene::OnCurrentTimeChanged(TSharedPtr<FFrameTime> InCurrentTime)
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		const FFrameRate TickResolution = DB->GetData()->TickResolution;
		float PreviewScrubTime = TickResolution.AsSeconds(*InCurrentTime);
		// UE_LOG(LogNovaAct, Log, TEXT("InCurrentTime : %f"), PreviewScrubTime);
		ActActionSkeletalMesh->PreviewInstance->SetPosition(PreviewScrubTime);
		ActActionSkeletalMesh->PreviewInstance->UpdateAnimation(PreviewScrubTime, true, UAnimInstance::EUpdateAnimationFlag::ForceParallelUpdate);
	}
}

void FActViewportPreviewScene::EvaluateToOneEnd(bool bIsEndEnd)
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
			ActEventTimelineArgs->CurrentTime->FrameNumber = TickResolution.AsFrameNumber(ActEventTimelineArgs->ClampRange.GetUpperBoundValue());
		}
		NovaDB::Trigger("ActEventTimelineArgs/CurrentTime");
	}
}

void FActViewportPreviewScene::OnPlaybackModeChanged(EPlaybackMode::Type InPlaybackMode) const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		UE_LOG(LogNovaAct, Log, TEXT("InPlaybackMode : %d"), InPlaybackMode);
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

void FActViewportPreviewScene::ToggleLooping() const
{
	if (UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		const bool bLoppingState = !PreviewInstanceLooping->GetData();
		PreviewInstance->SetLooping(bLoppingState);
		PreviewInstanceLooping->SetData(bLoppingState);
	}
}

bool FActViewportPreviewScene::IsLoopStatusOn() const
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->PreviewInstance)
	{
		return ActActionSkeletalMesh->PreviewInstance->IsLooping();
	}
	return false;
}

void FActViewportPreviewScene::PlayStep(bool bForward) const
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

// EPlaybackMode::Type FActViewportPreviewScene::GetPlaybackMode() const
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

float FActViewportPreviewScene::GetCurrentPosition() const
{
	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		return PreviewInstance->GetCurrentTime();
	}
	return 0.0f;
}

UAnimSingleNodeInstance* FActViewportPreviewScene::GetAnimSingleNodeInstance() const
{
	if (ActActionSkeletalMesh)
	{
		return Cast<UAnimSingleNodeInstance>(ActActionSkeletalMesh->PreviewInstance);
	}
	return nullptr;
}

void FActViewportPreviewScene::TickCurrentTimeChanged()
{
	if (const UAnimSingleNodeInstance* PreviewInstance = GetAnimSingleNodeInstance())
	{
		float CurrentTime = PreviewInstance->GetCurrentTime();
		if (CurrentTime != LastCurrentTime)
		{
			LastCurrentTime = CurrentTime;
			auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
			ActEventTimelineArgs->CurrentTime->FrameNumber = ActEventTimelineArgs->TickResolution.AsFrameNumber(CurrentTime);
			ActEventTimelineArgsDB->SetData(ActEventTimelineArgs);
		}
	}
}

void FActViewportPreviewScene::TickPlayingStopped()
{
	if (ActActionSkeletalMesh && ActActionSkeletalMesh->IsPreviewOn())
	{
		if (!ActActionSkeletalMesh->PreviewInstance->IsLooping() && !ActActionSkeletalMesh->PreviewInstance->IsPlaying())
		{
			PreviewInstancePlaybackMode->SetData(EPlaybackMode::Stopped);
		}
	}
}

void FActViewportPreviewScene::OnAnimBlueprintChanged(UActAnimation* InActAnimation)
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

void FActViewportPreviewScene::OnAnimSequenceChanged(UActAnimation* InActAnimation)
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
				NovaDB::Trigger("PreviewInstancePlaybackMode");
				ActActionSkeletalMesh->PreviewInstance->SetLooping(true);
				PreviewInstanceLooping->SetData(true);
				ActActionSkeletalMesh->SetPlayRate(1.0f);

				if (ActViewport.IsValid())
				{
					//Place the camera at a good viewer position
					const TSharedPtr<FEditorViewportClient> ViewportClient = ActViewport->GetViewportClient();
					FVector NewPosition = ViewportClient->GetViewLocation();
					NewPosition.Normalize();
					ViewportClient->SetViewLocation(NewPosition * (PreviewMesh->GetImportedBounds().SphereRadius * 1.5f));
				}
			}
		}
	}
}

void FActViewportPreviewScene::OnTransportControlsStateChanged(ENovaTransportControls InNovaTransportControls)
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