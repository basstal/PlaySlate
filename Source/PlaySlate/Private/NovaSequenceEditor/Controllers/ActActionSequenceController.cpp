#include "ActActionSequenceController.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Assets/Tracks/ActActionTrackEditorBase.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewScene.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceDisplayNode.h"
#include "NovaSequenceEditor/Widgets/ActActionSequenceWidget.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"

#include "Utils/ActActionStaticUtil.h"
#include "Utils/ActActionPlaybackUtil.h"

#include "IContentBrowserSingleton.h"
#include "LevelEditorViewport.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "Animation/SkeletalMeshActor.h"
#include "AnimPreviewInstance.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionSequenceController::FActActionSequenceController(UActActionSequence* InActActionSequence)
	: ActActionSequencePtr(InActActionSequence),
	  TargetViewRange(0.f, 5.f),
	  LastViewRange(0.f, 5.f),
	  PlaybackState(),
	  bNeedsEvaluate(false),
	  LocalLoopIndexOnBeginScrubbing(0),
	  LocalLoopIndexOffsetDuringScrubbing(0),
	  PreviewScrubTime(0),
	  PreviewActor(nullptr),
	  NodeTree(nullptr)
{
}

FActActionSequenceController::~FActActionSequenceController()
{
	ActActionViewportWidget.Reset();
	TimeSliderController.Reset();
}

void FActActionSequenceController::Tick(float DeltaTime)
{
	// Ensure the time bases for our playback position are kept up to date with the root sequence
	UpdateTimeBases();

	UpdateAnimInstance(DeltaTime);

	if (PreviewActor)
	{
		FActorTickFunction TickFunction;
		// PreviewActor->TickActor(DeltaTime, ELevelTick::LEVELTICK_ViewportsOnly, TickFunction);
	}
}

void FActActionSequenceController::UpdateAnimInstance(float DeltaTime)
{
	if (PreviewActor)
	{
		// UDebugSkelMeshComponent* MeshComponent = ActActionViewportWidget->GetPreviewScenePtr()->GetActActionSkeletalMesh();
		// UAnimInstance* AnimInstance = MeshComponent->PreviewInstance;
		// UAnimMontage* PlayingMontage = ActActionSequencePtr->EditAnimMontage;
		// FAnimMontageInstance* MontageInstanceToUpdate = (AnimInstance && PlayingMontage) ? AnimInstance->GetActiveInstanceForMontage(PlayingMontage) : nullptr;
		// // Now force the animation system to update, if we have a montage instance
		// if (MontageInstanceToUpdate)
		// {
		// 	MontageInstanceToUpdate->Pause();
		// 	AnimInstance->UpdateAnimation(DeltaTime, true, UAnimInstance::EUpdateAnimationFlag::ForceParallelUpdate);
		//
		// 	// since we don't advance montage in the tick, we manually have to handle notifies
		// 	// MontageInstanceToUpdate->HandleEvents(PreviousPosition, InPosition, NULL);
		//
		// 	// if (!bFireNotifies)
		// 	// {
		// 	// 	AnimInstance->NotifyQueue.Reset(MeshComponent);
		// 	// }
		//
		// 	// Allow the proxy to update (this also filters unfiltered notifies)
		// 	if (AnimInstance->NeedsUpdate())
		// 	{
		// 		AnimInstance->ParallelUpdateAnimation();
		// 	}
		//
		// 	// Explicitly call post update (also triggers notifies)
		// 	AnimInstance->PostUpdateAnimation();
		// }
		//
		// // Update space bases so new animation position has an effect.
		// MeshComponent->RefreshBoneTransforms();
		// MeshComponent->RefreshSlaveComponents();
		// MeshComponent->UpdateComponentToWorld();
		// MeshComponent->FinalizeBoneTransform();
		// MeshComponent->MarkRenderTransformDirty();
		// MeshComponent->MarkRenderDynamicDataDirty();
	}
}

TStatId FActActionSequenceController::GetStatId() const
{
	// ** TODO:STATGROUP_Tickables临时的标记
	RETURN_QUICK_DECLARE_CYCLE_STAT(FActActionSequenceController, STATGROUP_Tickables);
}

AActor* FActActionSequenceController::SpawnActorInViewport(UClass* ActorType)
{
	if (ActActionViewportWidget->GetPreviewScenePtr().IsValid())
	{
		return ActActionViewportWidget->GetPreviewScenePtr()->GetWorld()->SpawnActor(ActorType);
	}
	return nullptr;
}

void FActActionSequenceController::InitController(const TSharedRef<SWidget>& ViewWidget, const TArray<ActActionSequence::OnCreateTrackEditorDelegate>& TrackEditorDelegates, const TSharedRef<SActActionSequenceWidget>& InSequenceWidget)
{
	SequenceWidget = InSequenceWidget;
	ActActionViewportWidget = StaticCastSharedRef<SActActionViewportWidget>(ViewWidget);

	// Create tools and bind them to this sequence
	for (int32 DelegateIndex = 0; DelegateIndex < TrackEditorDelegates.Num(); ++DelegateIndex)
	{
		check(TrackEditorDelegates[DelegateIndex].IsBound());
		// Tools may exist in other modules, call a delegate that will create one for us 
		TSharedRef<FActActionTrackEditorBase> TrackEditor = TrackEditorDelegates[DelegateIndex].Execute(SharedThis(this));

		// if (TrackEditor->SupportsSequence(InActActionSequence))
		// {
		TrackEditors.Add(TrackEditor);
		// }
	}

	UpdateTimeBases();
	PlayPosition.Reset(GetPlaybackRange().GetLowerBoundValue());
}

void FActActionSequenceController::UpdateTimeBases()
{
	if (ActActionSequencePtr)
	{
		// EMovieSceneEvaluationType EvaluationType  = RootMovieScene->GetEvaluationType();
		FFrameRate TickResolution = ActActionSequencePtr->TickResolution;
		FFrameRate DisplayRate = ActActionSequencePtr->DisplayRate;

		if (DisplayRate != PlayPosition.GetInputRate())
		{
			bNeedsEvaluate = true;
		}

		// We set the play position in terms of the display rate,
		// but want evaluation ranges in the sequence's tick resolution
		PlayPosition.SetTimeBase(DisplayRate, TickResolution);
	}
}

void FActActionSequenceController::BuildAddObjectBindingsMenu(FMenuBuilder& MenuBuilder)
{
}

void FActActionSequenceController::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AddFolder", "Add Folder"),
		LOCTEXT("AddFolderToolTip", "Adds a new folder"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
		FUIAction(FExecuteAction::CreateRaw(this, &FActActionSequenceController::OnAddFolder))
	);
	for (int32 i = 0; i < TrackEditors.Num(); ++i)
	{
		// if (TrackEditors[i]->SupportsSequence(ActActionSequencePtr))
		// {
		TrackEditors[i]->BuildAddTrackMenu(MenuBuilder);
		// }
	}
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetViewRange() const
{
	// float LowerRange = FMath::Lerp(LastViewRange.GetLowerBoundValue(), TargetViewRange.GetLowerBoundValue(), ZoomCurve.GetLerp());
	// float HigherRange = FMath::Lerp(LastViewRange.GetUpperBoundValue(), TargetViewRange.GetUpperBoundValue(), ZoomCurve.GetLerp());
	ActActionSequence::FActActionAnimatedRange AnimatedRange(TargetViewRange.GetLowerBoundValue(), TargetViewRange.GetUpperBoundValue());
	if (ZoomAnimation.IsPlaying())
	{
		AnimatedRange.AnimationTarget = TargetViewRange;
	}

	return AnimatedRange;
}

bool FActActionSequenceController::IsReadOnly() const
{
	return false;
}

FFrameRate FActActionSequenceController::GetFocusedTickResolution() const
{
	UActActionSequence* FocusedSequence = GetActActionSequencePtr();
	if (FocusedSequence)
	{
		return FocusedSequence->TickResolution;
	}

	ensureMsgf(false, TEXT("No valid sequence found."));
	return FFrameRate();
}

FFrameRate FActActionSequenceController::GetFocusedDisplayRate() const
{
	UActActionSequence* FocusedSequence = GetActActionSequencePtr();
	if (FocusedSequence)
	{
		return FocusedSequence->DisplayRate;
	}

	ensureMsgf(false, TEXT("No valid sequence found."));
	return FFrameRate();
}

void FActActionSequenceController::OnAddFolder()
{
	// FAssetPickerConfig AssetPickerConfig;
	// AssetPickerConfig.Filter.ClassNames.Add(FName("111"));
	// InAssetPickerConfig = AssetPickerConfig;
	UE_LOG(LogActAction, Log, TEXT("Nothing happened"));
}

void FActActionSequenceController::RequestListRefresh()
{
	if (SequenceWidget.IsValid())
	{
		SequenceWidget->GetTreeView()->RequestListRefresh();
	}
}

void FActActionSequenceController::AddRootNodes(TSharedPtr<FActActionSequenceDisplayNode> SequenceDisplayNode)
{
	if (SequenceWidget.IsValid())
	{
		TSharedPtr<SActActionSequenceTreeView> SequenceTreeView = SequenceWidget->GetTreeView();
		SequenceTreeView->AddRootNodes(SequenceDisplayNode);
		SequenceTreeView->SetTreeItemsSource(SequenceTreeView->GetRootNodes());
	}
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetClampRange() const
{
	// return GetFocusedMovieSceneSequence()->GetMovieScene()->GetEditorData().GetWorkingRange();
	return ActActionSequence::FActActionAnimatedRange();
}

TRange<FFrameNumber> FActActionSequenceController::GetPlaybackRange() const
{
	// return GetFocusedMovieSceneSequence()->GetMovieScene()->GetPlaybackRange();
	return ActActionSequencePtr->PlaybackRange;
}

ActActionSequence::EPlaybackType FActActionSequenceController::GetPlaybackStatus() const
{
	return PlaybackState;
}

TRange<FFrameNumber> FActActionSequenceController::GetSelectionRange() const
{
	return ActActionSequencePtr->SelectionRange;
}

void FActActionSequenceController::AddAnimMontageTrack(UAnimMontage* AnimMontage)
{
	if (AnimMontage)
	{
		UE_LOG(LogActAction, Log, TEXT("AnimMontage : %s"), *AnimMontage->GetName());
		ActActionSequencePtr->EditAnimMontage = AnimMontage;
		// ** 添加左侧Track
		AddRootNodes(MakeShareable(new FActActionSequenceDisplayNode(GetNodeTree())));
		if (TimeSliderController.IsValid())
		{
			// ** 添加右侧TimeSlider
			float CalculateSequenceLength = AnimMontage->CalculateSequenceLength();
			UE_LOG(LogActAction, Log, TEXT("CalculateSequenceLength : %f"), CalculateSequenceLength);
			// ** 限制显示的最大长度为当前的Sequence总时长
			TargetViewRange = TRange<double>(0, CalculateSequenceLength);
			FFrameNumber EndFrame = FFrameNumber((int32)(CalculateSequenceLength * TimeSliderController->GetTimeSliderArgs().TickResolution.Get().Numerator));
			TimeSliderController->SetPlaybackRangeEnd(EndFrame);
		}


		UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(AnimMontage);
		USkeleton* Skeleton = nullptr;
		if (AnimSequence)
		{
			Skeleton = AnimSequence->GetSkeleton();
		}

		UDebugSkelMeshComponent* PreviewComponent = ActActionViewportWidget->GetPreviewScenePtr()->GetActActionSkeletalMesh();
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

		// TargetSkeleton = Skeleton;
	}
}

void FActActionSequenceController::SetPlaybackStatus(ActActionSequence::EPlaybackType InPlaybackStatus)
{
	PlaybackState = InPlaybackStatus;
	PauseOnFrame.Reset();

	// Inform the renderer when Sequencer is in a 'paused' state for the sake of inter-frame effects
	ESequencerState SequencerState = ESS_None;
	if (InPlaybackStatus == ActActionSequence::EPlaybackType::Playing || InPlaybackStatus == ActActionSequence::EPlaybackType::Recording)
	{
		SequencerState = ESS_Playing;
	}
	else if (InPlaybackStatus == ActActionSequence::EPlaybackType::Stopped || InPlaybackStatus == ActActionSequence::EPlaybackType::Scrubbing || InPlaybackStatus == ActActionSequence::EPlaybackType::Stepping)
	{
		SequencerState = ESS_Paused;
	}

	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC && LevelVC->AllowsCinematicControl())
		{
			LevelVC->ViewState.GetReference()->SetSequencerState(SequencerState);
		}
	}

	// if (InPlaybackStatus == ActActionSequence::EPlaybackType::Playing)
	// {
	// 	// if (Settings->GetCleanPlaybackMode())
	// 	// {
	// 	// 	CachedViewState.StoreViewState();
	// 	// }
	//
	// 	// override max frame rate
	// 	// if (PlayPosition.GetEvaluationType() == EMovieSceneEvaluationType::FrameLocked)
	// 	// {
	// 	// 	if (!OldMaxTickRate.IsSet())
	// 	// 	{
	// 	// 		OldMaxTickRate = GEngine->GetMaxFPS();
	// 	// 	}
	// 	//
	// 	// 	GEngine->SetMaxFPS(1.f / PlayPosition.GetInputRate().AsInterval());
	// 	// }
	// }
	// else
	// {
	// 	// CachedViewState.RestoreViewState();
	// 	//
	// 	// StopAutoscroll();
	// 	//
	// 	// if (OldMaxTickRate.IsSet())
	// 	// {
	// 	// 	GEngine->SetMaxFPS(OldMaxTickRate.GetValue());
	// 	// 	OldMaxTickRate.Reset();
	// 	// }
	// 	//
	// 	// ShuttleMultiplier = 0;
	// }

	// TimeController->PlayerStatusChanged(PlaybackState, GetGlobalTime());
}


void FActActionSequenceController::EvaluateInternal(ActActionSequence::FActActionEvaluationRange InRange, bool bHasJumped)
{
	// if (Settings->ShouldCompileDirectorOnEvaluate())
	// {
	// 	RecompileDirtyDirectors();
	// }

	bNeedsEvaluate = false;
	//
	// UpdateCachedPlaybackContextAndClient();
	//
	// if (EventContextsAttribute.IsBound())
	// {
	// 	CachedEventContexts.Reset();
	// 	for (UObject* Object : EventContextsAttribute.Get())
	// 	{
	// 		CachedEventContexts.Add(Object);
	// 	}
	// }
	//
	// FMovieSceneContext Context = FMovieSceneContext(InRange, PlaybackState).SetIsSilent(SilentModeCount != 0);
	// Context.SetHasJumped(bHasJumped);
	//
	// FMovieSceneSequenceID RootOverride = MovieSceneSequenceID::Root;
	// if (Settings->ShouldEvaluateSubSequencesInIsolation())
	// {
	// 	RootOverride = ActiveTemplateIDs.Top();
	// }
	//
	// RootTemplateInstance.Evaluate(Context, *this);
	// SuppressAutoEvalSignature.Reset();
	//
	// if (RootTemplateInstance.GetEntitySystemRunner().IsAttachedToLinker())
	// {
	// 	RootTemplateInstance.GetEntitySystemRunner().Flush();
	// }
	//
	// if (Settings->ShouldRerunConstructionScripts())
	// {
	// 	RerunConstructionScripts();
	// }
	//
	// if (!IsInSilentMode())
	// {
	// 	OnGlobalTimeChangedDelegate.Broadcast();
	// }
	if (ActActionViewportWidget.IsValid())
	{
		FFrameTime CurrentPosition = InRange.GetTime();
		TSharedPtr<FActActionPreviewScene> PreviewScene = ActActionViewportWidget->GetPreviewScenePtr();
		if (PreviewScene.IsValid())
		{
			UDebugSkelMeshComponent* PreviewMeshComponent = PreviewScene->GetActActionSkeletalMesh();
			if (PreviewMeshComponent && PreviewMeshComponent->IsPreviewOn())
			{
				if (PreviewMeshComponent->PreviewInstance->IsPlaying())
				{
					PreviewMeshComponent->PreviewInstance->SetPlaying(false);
				}
				PreviewScrubTime = CurrentPosition.AsDecimal() / 60;
				PreviewMeshComponent->PreviewInstance->SetPosition(PreviewScrubTime);
				PreviewMeshComponent->PreviewInstance->UpdateAnimation(PreviewScrubTime, false);
			}
		}
	}
}

void FActActionSequenceController::Pause()
{
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);

	// When stopping a sequence, we always evaluate a non-empty range if possible. This ensures accurate paused motion blur effects.
	// if (Settings->GetIsSnapEnabled())
	// {
	// 	FQualifiedFrameTime LocalTime          = GetLocalTime();
	// 	FFrameRate          FocusedDisplayRate = GetFocusedDisplayRate();
	//
	// 	// Snap to the focused play rate
	// 	FFrameTime RootPosition  = FFrameRate::Snap(LocalTime.Time, LocalTime.Rate, FocusedDisplayRate) * RootToLocalTransform.InverseFromWarp(RootToLocalLoopCounter);
	//
	// 	// Convert the root position from tick resolution time base (the output rate), to the play position input rate
	// 	FFrameTime InputPosition = ConvertFrameTime(RootPosition, PlayPosition.GetOutputRate(), PlayPosition.GetInputRate());
	// 	EvaluateInternal(PlayPosition.PlayTo(InputPosition));
	// }
	// else
	// {
	// Update on stop (cleans up things like sounds that are playing)
	ActActionSequence::FActActionEvaluationRange Range = PlayPosition.GetLastRange().Get(PlayPosition.GetCurrentPositionAsRange());
	EvaluateInternal(Range);
	// }

	// OnStopDelegate.Broadcast();
}

void FActActionSequenceController::StopAutoscroll()
{
	AutoscrollOffset.Reset();
	AutoScrubOffset.Reset();
}


TSet<FFrameNumber> FActActionSequenceController::GetVerticalFrames() const
{
	TSet<FFrameNumber> VerticalFrames;

	// auto AddVerticalFrames = [](auto &InVerticalFrames, auto InTrack) 
	// {
	// 	for (UMovieSceneSection* Section : InTrack->GetAllSections())
	// 	{
	// 		if (Section->GetRange().HasLowerBound())
	// 		{
	// 			InVerticalFrames.Add(Section->GetRange().GetLowerBoundValue());
	// 		}
	//
	// 		if (Section->GetRange().HasUpperBound())
	// 		{
	// 			InVerticalFrames.Add(Section->GetRange().GetUpperBoundValue());
	// 		}
	// 	}
	// };

	// UMovieSceneSequence* FocusedMovieSequence = GetFocusedMovieSceneSequence();
	// if (FocusedMovieSequence != nullptr)
	// {
	// 	UMovieScene* FocusedMovieScene = FocusedMovieSequence->GetMovieScene();
	// 	if (FocusedMovieScene != nullptr)
	// 	{
	// 		for (UMovieSceneTrack* MasterTrack : FocusedMovieScene->GetMasterTracks())
	// 		{
	// 			if (MasterTrack && MasterTrack->DisplayOptions.bShowVerticalFrames)
	// 			{
	// 				AddVerticalFrames(VerticalFrames, MasterTrack);
	// 			}
	// 		}
	//
	// 		if (UMovieSceneTrack* CameraCutTrack = FocusedMovieScene->GetCameraCutTrack())
	// 		{
	// 			if (CameraCutTrack->DisplayOptions.bShowVerticalFrames)
	// 			{
	// 				AddVerticalFrames(VerticalFrames, CameraCutTrack);
	// 			}
	// 		}
	// 	}
	// }

	return VerticalFrames;
}


void FActActionSequenceController::SetMarkedFrame(int32 InMarkIndex, FFrameNumber InFrameNumber)
{
	// UMovieSceneSequence* FocusedMovieSequence = GetFocusedMovieSceneSequence();
	if (ActActionSequencePtr)
	{
		// UMovieScene* FocusedMovieScene = FocusedMovieSequence->GetMovieScene();
		// if (FocusedMovieScene != nullptr)
		// {
		ActActionSequencePtr->Modify();
		// ** TODO:
		// ActActionSequencePtr->SetMarkedFrame(InMarkIndex, InFrameNumber);
		// }
	}
}

void FActActionSequenceController::AddMarkedFrame(FFrameNumber FrameNumber)
{
	// UMovieSceneSequence* FocusedMovieSequence = GetFocusedMovieSceneSequence();
	// if (FocusedMovieSequence != nullptr)
	// {
	// 	UMovieScene* FocusedMovieScene = FocusedMovieSequence->GetMovieScene();
	if (ActActionSequencePtr)
	{
		FScopedTransaction AddMarkedFrameTransaction(LOCTEXT("AddMarkedFrame_Transaction", "Add Marked Frame"));

		ActActionSequencePtr->Modify();
		// ** TODO:
		// ActActionSequencePtr->AddMarkedFrame(FMovieSceneMarkedFrame(FrameNumber));
	}
	// }
}


void FActActionSequenceController::SetPlaybackRange(TRange<FFrameNumber> Range)
{
	if (ensure(Range.HasLowerBound() && Range.HasUpperBound()))
	{
		// if (!IsPlaybackRangeLocked())
		// {
		// UMovieScene* FocusedMovieScene = GetFocusedMovieSceneSequence()->GetMovieScene();
		if (ActActionSequencePtr)
		{
			const FScopedTransaction Transaction(LOCTEXT("SetPlaybackRange_Transaction", "Set Playback Range"));
			ActActionSequencePtr->PlaybackRange = Range;

			// bNeedsEvaluate = true;
			// NotifyMovieSceneDataChanged(EMovieSceneDataChangeType::TrackValueChanged);
		}
		// }
	}
}

void FActActionSequenceController::InitAnimBlueprint(UAnimBlueprint* AnimBlueprint)
{
	if (AnimBlueprint && AnimBlueprint->TargetSkeleton)
	{
		if (ActActionSequencePtr->EditAnimBlueprint != AnimBlueprint)
		{
			UE_LOG(LogActAction, Log, TEXT("AssignAsEditAnim PreviewActor : %s"), *AnimBlueprint->GetName());
			ActActionSequencePtr->EditAnimBlueprint = AnimBlueprint;
		}
		ASkeletalMeshActor* InPreviewActor = Cast<ASkeletalMeshActor>(SpawnActorInViewport(ASkeletalMeshActor::StaticClass()));
		if (InPreviewActor)
		{
			PreviewActor = InPreviewActor;
			if (ActActionViewportWidget.IsValid() && ActActionViewportWidget->GetPreviewScenePtr().IsValid())
			{
				ActActionViewportWidget->GetPreviewScenePtr()->InitPreviewScene(PreviewActor);
			}
			USkeletalMeshComponent* MeshComponent = ActActionViewportWidget->GetPreviewScenePtr()->GetActActionSkeletalMesh();
			MeshComponent->OverrideMaterials.Empty();

			USkeletalMesh* PreviewSkeletalMesh = AnimBlueprint->TargetSkeleton->GetPreviewMesh(true);
			MeshComponent->SetSkeletalMesh(PreviewSkeletalMesh);

			UAnimInstance* PreviousAnimInstance = MeshComponent->GetAnimInstance();
			MeshComponent->SetAnimInstanceClass(AnimBlueprint->GeneratedClass);
			MeshComponent->InitializeAnimScriptInstance();

			if (PreviousAnimInstance && PreviousAnimInstance != MeshComponent->GetAnimInstance())
			{
				//Mark this as gone!
				PreviousAnimInstance->MarkPendingKill();
			}

			PreviewActor->SetActorLocation(FVector(0, 0, 0), false);
			MeshComponent->UpdateBounds();

			// Center the mesh at the world origin then offset to put it on top of the plane
			const float BoundsZOffset = ActActionSequence::ActActionStaticUtil::GetBoundsZOffset(MeshComponent->Bounds);
			PreviewActor->SetActorLocation(-MeshComponent->Bounds.Origin + FVector(0, 0, BoundsZOffset), false);
			MeshComponent->RecreateRenderState_Concurrent();
		}
	}
}

FFrameTime FActActionSequenceController::GetLocalFrameTime() const
{
	return GetLocalTime().Time;
}

FQualifiedFrameTime FActActionSequenceController::GetLocalTime() const
{
	const FFrameRate FocusedResolution = GetFocusedTickResolution();
	const FFrameTime CurrentPosition = PlayPosition.GetCurrentPosition();

	const FFrameTime RootTime = ConvertFrameTime(CurrentPosition, PlayPosition.GetInputRate(), PlayPosition.GetOutputRate());
	return FQualifiedFrameTime(RootTime, FocusedResolution);
}


FString FActActionSequenceController::GetFrameTimeText() const
{
	const FFrameTime CurrentPosition = PlayPosition.GetCurrentPosition();

	const FFrameTime RootTime = ConvertFrameTime(CurrentPosition, PlayPosition.GetInputRate(), PlayPosition.GetOutputRate());

	const FFrameTime LocalTime = RootTime;

	return SequenceWidget->GetNumericType()->ToString(LocalTime.GetFrame().Value);
}

uint32 FActActionSequenceController::GetLocalLoopIndex() const
{
	// if (RootToLocalLoopCounter.WarpCounts.Num() == 0)
	// {
	// 	return FMovieSceneTimeWarping::InvalidWarpCount;
	// }
	// else
	// {
	// const bool bIsScrubbing = GetPlaybackStatus() == ActActionSequence::EPlaybackType::Scrubbing;
	// return RootToLocalLoopCounter.WarpCounts.Last() + (bIsScrubbing ? LocalLoopIndexOffsetDuringScrubbing : 0);
	// }
	return 0;
}

void FActActionSequenceController::OnBeginScrubbing()
{
	// Pause first since there's no explicit evaluation in the stopped state when OnEndScrubbing() is called
	Pause();

	SetPlaybackStatus(ActActionSequence::EPlaybackType::Scrubbing);
	// SequenceWidget->RegisterActiveTimerForPlayback();

	LocalLoopIndexOnBeginScrubbing = GetLocalLoopIndex();
	LocalLoopIndexOffsetDuringScrubbing = 0;

	// OnBeginScrubbingDelegate.Broadcast();
}


void FActActionSequenceController::OnEndScrubbing()
{
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);
	StopAutoscroll();

	LocalLoopIndexOnBeginScrubbing = -1;
	LocalLoopIndexOffsetDuringScrubbing = 0;

	// OnEndScrubbingDelegate.Broadcast();
}


void FActActionSequenceController::SetGlobalTime(FFrameTime NewTime)
{
	NewTime = ConvertFrameTime(NewTime, ActActionSequencePtr->TickResolution, PlayPosition.GetInputRate());
	// if (PlayPosition.GetEvaluationType() == EMovieSceneEvaluationType::FrameLocked)
	// {
	// 	NewTime = NewTime.FloorToFrame();
	// }

	// Don't update the sequence if the time hasn't changed as this will cause duplicate events and the like to fire.
	// If we need to reevaluate the sequence at the same time for whatever reason, we should call ForceEvaluate()
	TOptional<FFrameTime> CurrentPosition = PlayPosition.GetCurrentPosition();
	if (PlayPosition.GetCurrentPosition() != NewTime)
	{
		EvaluateInternal(PlayPosition.JumpTo(NewTime));
	}

	if (AutoScrubTarget.IsSet())
	{
		SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);
		AutoScrubTarget.Reset();
	}
}

void FActActionSequenceController::SetLocalTimeDirectly(FFrameTime NewTime)
{
	TWeakPtr<SWidget> PreviousFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();

	// Clear focus before setting time in case there's a key editor value selected that gets committed to a newly selected key on UserMovedFocus
	if (GetPlaybackStatus() == ActActionSequence::EPlaybackType::Stopped)
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}

	// Transform the time to the root time-space
	SetGlobalTime(NewTime);

	if (PreviousFocusedWidget.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(PreviousFocusedWidget.Pin());
	}
}

void FActActionSequenceController::OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing)
{
	if (PlaybackState == ActActionSequence::EPlaybackType::Scrubbing)
	{
		if (!bScrubbing)
		{
			OnEndScrubbing();
		}
		// else if (IsAutoScrollEnabled())
		// {
		// 	UpdateAutoScroll(NewScrubPosition / GetFocusedTickResolution());
		// 	
		// 	// When scrubbing, we animate auto-scrolled scrub position in Tick()
		// 	if (AutoScrubOffset.IsSet())
		// 	{
		// 		return;
		// 	}
		// }
	}

	if (!bScrubbing && FSlateApplication::Get().GetModifierKeys().IsShiftDown())
	{
		AutoScrubTarget = ActActionSequence::FActActionAutoScrubTarget(NewScrubPosition, GetLocalTime().Time, FPlatformTime::Seconds());
	}
	else
	{
		SetLocalTimeDirectly(NewScrubPosition);
	}
}


TSharedPtr<SActActionSequenceWidget> FActActionSequenceController::MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams)
{
	TSharedPtr<SActActionSequenceWidget> ActActionSequenceWidget = SNew(SActActionSequenceWidget, SharedThis(this))
		.ViewRange(this, &FActActionSequenceController::GetViewRange)
		.ClampRange(this, &FActActionSequenceController::GetClampRange)
		.PlaybackRange(this, &FActActionSequenceController::GetPlaybackRange)
		.PlaybackStatus(this, &FActActionSequenceController::GetPlaybackStatus)
		.SelectionRange(this, &FActActionSequenceController::GetSelectionRange)
		.VerticalFrames(this, &FActActionSequenceController::GetVerticalFrames)
		// .MarkedFrames(this, &FActActionSequenceController::GetMarkedFrames)
		// .GlobalMarkedFrames(this, &FActActionSequenceController::GetGlobalMarkedFrames)
		.OnSetMarkedFrame(this, &FActActionSequenceController::SetMarkedFrame)
		.OnAddMarkedFrame(this, &FActActionSequenceController::AddMarkedFrame)
		// .OnDeleteMarkedFrame(this, &FActActionSequenceController::DeleteMarkedFrame)
		// .OnDeleteAllMarkedFrames(this, &FActActionSequenceController::DeleteAllMarkedFrames)
		// .SubSequenceRange(this, &FActActionSequenceController::GetSubSequenceRange)
		.OnPlaybackRangeChanged(this, &FActActionSequenceController::SetPlaybackRange)
		// .OnPlaybackRangeBeginDrag(this, &FActActionSequenceController::OnPlaybackRangeBeginDrag)
		// .OnPlaybackRangeEndDrag(this, &FActActionSequenceController::OnPlaybackRangeEndDrag)
		// .OnSelectionRangeChanged(this, &FActActionSequenceController::SetSelectionRange)
		// .OnSelectionRangeBeginDrag(this, &FActActionSequenceController::OnSelectionRangeBeginDrag)
		// .OnSelectionRangeEndDrag(this, &FActActionSequenceController::OnSelectionRangeEndDrag)
		// .OnMarkBeginDrag(this, &FActActionSequenceController::OnMarkBeginDrag)
		// .OnMarkEndDrag(this, &FActActionSequenceController::OnMarkEndDrag)
		// .IsPlaybackRangeLocked(this, &FActActionSequenceController::IsPlaybackRangeLocked)
		// .OnTogglePlaybackRangeLocked(this, &FActActionSequenceController::TogglePlaybackRangeLocked)
		.ScrubPosition(this, &FActActionSequenceController::GetLocalFrameTime)
		.ScrubPositionText(this, &FActActionSequenceController::GetFrameTimeText)
		// .ScrubPositionParent(this, &FActActionSequenceController::GetScrubPositionParent)
		// .ScrubPositionParentChain(this, &FActActionSequenceController::GetScrubPositionParentChain)
		// .OnScrubPositionParentChanged(this, &FActActionSequenceController::OnScrubPositionParentChanged)
		.OnBeginScrubbing(this, &FActActionSequenceController::OnBeginScrubbing)
		.OnEndScrubbing(this, &FActActionSequenceController::OnEndScrubbing)
		.OnScrubPositionChanged(this, &FActActionSequenceController::OnScrubPositionChanged)
		// .OnViewRangeChanged(this, &FActActionSequenceController::SetViewRange)
		// .OnClampRangeChanged(this, &FActActionSequenceController::OnClampRangeChanged)
		// .OnGetNearestKey(this, &FActActionSequenceController::OnGetNearestKey)
		.OnGetAddMenuContent(ViewParams.OnGetAddMenuContent)
		// .OnBuildCustomContextMenuForGuid(ViewParams.OnBuildCustomContextMenuForGuid)
		.OnReceivedFocus(ViewParams.OnReceivedFocus)
		.AddMenuExtender(ViewParams.AddMenuExtender)
		.ToolbarExtender(ViewParams.ToolbarExtender);
	// ** 将内部Widget的Controller同步到最外层的Controller中
	TimeSliderController = ActActionSequenceWidget->TimeSliderController;
	return ActActionSequenceWidget;
}


#undef LOCTEXT_NAMESPACE
