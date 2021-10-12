#include "ActActionSequenceController.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/Assets/Tracks/ActActionTrackEditorBase.h"
#include "NovaSequenceEditor/Assets/Tracks/ActActionAnimInstanceTrack.h"
#include "NovaSequenceEditor/Assets/Tracks/ActActionAnimMontageTrack.h"
#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Widgets/ActActionSequenceWidget.h"
#include "NovaSequenceEditor/Widgets/SequenceNodeTree/ActActionSequenceTreeView.h"

#include "Utils/ActActionPlaybackUtil.h"

#include "IContentBrowserSingleton.h"
#include "LevelEditorViewport.h"
#include "Animation/AnimInstance.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "AnimPreviewInstance.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionSequenceController::FActActionSequenceController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: ActActionSequenceEditor(InActActionSequenceEditor),
	  TargetViewRange(0.f, 5.f),
	  LastViewRange(0.f, 5.f),
	  PlaybackState(ActActionSequence::EPlaybackType::Stopped),
	  bNeedsEvaluate(false),
	  LocalLoopIndexOnBeginScrubbing(0),
	  LocalLoopIndexOffsetDuringScrubbing(0)
{
	PlayPosition.Reset(InActActionSequenceEditor->GetPlaybackRange().GetLowerBoundValue());
	// ** 将Sequence能编辑的所有TrackEditor注册，以便能够使用AddTrackEditor以及AddTrackMenu
	TrackEditorDelegates.Add(ActActionSequence::OnCreateTrackEditorDelegate::CreateStatic(&FActActionAnimInstanceTrack::CreateTrackEditor));
	TrackEditorDelegates.Add(ActActionSequence::OnCreateTrackEditorDelegate::CreateStatic(&FActActionAnimMontageTrack::CreateTrackEditor));
}

FActActionSequenceController::~FActActionSequenceController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionSequenceController::~FActActionSequenceController"));
	SequenceWidget.Reset();
	TimeSliderController.Reset();
	TreeViewRoot.Reset();
}

void FActActionSequenceController::Tick(float DeltaTime)
{
	// Ensure the time bases for our playback position are kept up to date with the root sequence
	UpdateTimeBases();
}

TStatId FActActionSequenceController::GetStatId() const
{
	// ** TODO:STATGROUP_Tickables临时的标记
	RETURN_QUICK_DECLARE_CYCLE_STAT(FActActionSequenceController, STATGROUP_Tickables);
}

void FActActionSequenceController::ExecuteTrackEditorCreateDelegate()
{
	// Create tools and bind them to this sequence
	for (int32 DelegateIndex = 0; DelegateIndex < TrackEditorDelegates.Num(); ++DelegateIndex)
	{
		check(TrackEditorDelegates[DelegateIndex].IsBound());
		// Tools may exist in other modules, call a delegate that will create one for us 
		TSharedRef<FActActionTrackEditorBase> TrackEditor = TrackEditorDelegates[DelegateIndex].Execute(SharedThis(this));
		TrackEditors.Add(TrackEditor);
	}
}

void FActActionSequenceController::UpdateTimeBases()
{
	if (UActActionSequence* ActActionSequence = GetActActionSequence())
	{
		FFrameRate TickResolution = ActActionSequence->TickResolution;
		FFrameRate DisplayRate = ActActionSequence->DisplayRate;

		if (DisplayRate != PlayPosition.GetInputRate())
		{
			bNeedsEvaluate = true;
		}

		// We set the play position in terms of the display rate,
		// but want evaluation ranges in the sequence's tick resolution
		PlayPosition.SetTimeBase(DisplayRate, TickResolution);
	}
}

void FActActionSequenceController::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	for (int32 i = 0; i < TrackEditors.Num(); ++i)
	{
		TrackEditors[i]->BuildAddTrackMenu(MenuBuilder);
	}
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetViewRange() const
{
	ActActionSequence::FActActionAnimatedRange AnimatedRange(TargetViewRange.GetLowerBoundValue(), TargetViewRange.GetUpperBoundValue());
	if (ZoomAnimation.IsPlaying())
	{
		AnimatedRange.AnimationTarget = TargetViewRange;
	}

	return AnimatedRange;
}

// TODO:这个接口改到Editor中，资源调用Editor接口再调用Controller
void FActActionSequenceController::AddAnimMontageTrack(UAnimMontage* AnimMontage)
{
	if (!AnimMontage)
	{
		UE_LOG(LogActAction, Error, TEXT("FActActionSequenceController::AddAnimMontageTrack with nullptr AnimMontage"))
		return;
	}
	if (UActActionSequence* ActActionSequence = GetActActionSequence())
	{
		UE_LOG(LogActAction, Log, TEXT("AnimMontage : %s"), *AnimMontage->GetName());
		ActActionSequence->EditAnimMontage = AnimMontage;
		// ** 添加左侧Track
		if (SequenceWidget.IsValid())
		{
			TSharedPtr<SActActionSequenceTreeView> SequenceTreeView = SequenceWidget->GetTreeView();
			SequenceTreeView->AddDisplayNode(MakeShareable(new FActActionSequenceTreeViewNode(SharedThis(this), TreeViewRoot)));
		}
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

		if (ActActionSequenceEditor.IsValid())
		{
			ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->InitAnimationByAnimMontage(AnimMontage);
		}
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
	bNeedsEvaluate = false;
}

void FActActionSequenceController::Pause()
{
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);

	// When stopping a sequence, we always evaluate a non-empty range if possible. This ensures accurate paused motion blur effects.
	// if (Settings->GetIsSnapEnabled())
	// {
	// 	FQualifiedFrameTime LocalTime          = GetLocalTime();
	// 	FFrameRate          FocusedDisplayRate = GetDisplayRate();
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
	if (UActActionSequence* ActActionSequence = GetActActionSequence())
	{
		// UMovieScene* FocusedMovieScene = FocusedMovieSequence->GetMovieScene();
		// if (FocusedMovieScene != nullptr)
		// {
		ActActionSequence->Modify();
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
	if (UActActionSequence* ActActionSequence = GetActActionSequence())
	{
		FScopedTransaction AddMarkedFrameTransaction(LOCTEXT("AddMarkedFrame_Transaction", "Add Marked Frame"));

		ActActionSequence->Modify();
		// ** TODO:
		// ActActionSequencePtr->AddMarkedFrame(FMovieSceneMarkedFrame(FrameNumber));
	}
	// }
}


FFrameTime FActActionSequenceController::GetLocalFrameTime() const
{
	return GetLocalTime().Time;
}

FQualifiedFrameTime FActActionSequenceController::GetLocalTime() const
{
	const FFrameRate FocusedResolution = GetActActionSequenceEditor()->GetTickResolution();
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
	if (!GetActActionSequence())
	{
		return;
	}
	NewTime = ConvertFrameTime(NewTime, GetActActionSequence()->TickResolution, PlayPosition.GetInputRate());
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
		// 	UpdateAutoScroll(NewScrubPosition / GetTickResolution());
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

UActActionSequence* FActActionSequenceController::GetActActionSequence() const
{
	if (ActActionSequenceEditor.IsValid())
	{
		return ActActionSequenceEditor.Pin()->GetActActionSequence();
	}
	return nullptr;
}

TSharedRef<FActActionSequenceEditor> FActActionSequenceController::GetActActionSequenceEditor() const
{
	return ActActionSequenceEditor.Pin().ToSharedRef();
}


void FActActionSequenceController::MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams)
{
	TSharedRef<FActActionSequenceController> ActionSequenceController = SharedThis(this);
	// ** 构造所有显示节点的根节点
	TreeViewRoot = MakeShareable(new FActActionSequenceTreeViewNode(ActionSequenceController));
	
	TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = ActActionSequenceEditor.Pin().ToSharedRef();
	TSharedPtr<SActActionSequenceWidget> ActActionSequenceWidget = SNew(SActActionSequenceWidget, ActionSequenceController)
		.ViewRange(this, &FActActionSequenceController::GetViewRange)
		.PlaybackRange(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetPlaybackRange)
		.PlaybackStatus(this, &FActActionSequenceController::GetPlaybackStatus)
		.SelectionRange(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetSelectionRange)
		.VerticalFrames(this, &FActActionSequenceController::GetVerticalFrames)
		// .MarkedFrames(this, &FActActionSequenceController::GetMarkedFrames)
		// .GlobalMarkedFrames(this, &FActActionSequenceController::GetGlobalMarkedFrames)
		.OnSetMarkedFrame(this, &FActActionSequenceController::SetMarkedFrame)
		.OnAddMarkedFrame(this, &FActActionSequenceController::AddMarkedFrame)
		// .OnDeleteMarkedFrame(this, &FActActionSequenceController::DeleteMarkedFrame)
		// .OnDeleteAllMarkedFrames(this, &FActActionSequenceController::DeleteAllMarkedFrames)
		// .SubSequenceRange(this, &FActActionSequenceController::GetSubSequenceRange)
		.OnPlaybackRangeChanged(ActActionSequenceEditorRef, &FActActionSequenceEditor::SetPlaybackRange)
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
	SequenceWidget = ActActionSequenceWidget;
}


#undef LOCTEXT_NAMESPACE
