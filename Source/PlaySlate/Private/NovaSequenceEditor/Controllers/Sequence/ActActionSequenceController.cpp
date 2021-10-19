#include "ActActionSequenceController.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Assets/Tracks/ActActionTrackEditorBase.h"
#include "NovaSequenceEditor/Controllers/Sequence/TimeSlider/ActActionTimeSliderController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaSequenceEditor/Widgets/Sequence/ActActionSequenceWidget.h"

#include "Utils/ActActionPlaybackUtil.h"

#include "IContentBrowserSingleton.h"
#include "LevelEditorViewport.h"
#include "Animation/AnimInstance.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "AnimPreviewInstance.h"
#include "FrameNumberNumericInterface.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionSequenceController::FActActionSequenceController(const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: ActActionSequenceEditor(InActActionSequenceEditor),
	  PlaybackState(ActActionSequence::EPlaybackType::Stopped),
	  TargetViewRange(0, 0),
	  TargetClampRange(0, 0)
{
	// ** 将Sequence能编辑的所有TrackEditor注册，以便能够使用AddTrackEditor以及AddTrackMenu
}

FActActionSequenceController::~FActActionSequenceController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionSequenceController::~FActActionSequenceController"));
	ActActionSequenceWidget.Reset();
	ActActionTimeSliderController.Reset();
	ActActionSequenceTreeViewNode.Reset();
	TrackEditors.Empty();
}


void FActActionSequenceController::MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams)
{
	// ** 构造所有显示节点的根节点
	ActActionSequenceTreeViewNode = MakeShareable(new FActActionSequenceTreeViewNode(SharedThis(this)));

	// Get the desired display format from the user's settings each time.
	const TAttribute<EFrameNumberDisplayFormats> GetDisplayFormatAttr = MakeAttributeLambda([=]
	{
		return EFrameNumberDisplayFormats::Frames;
	});
	const TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = ActActionSequenceEditor.Pin().ToSharedRef();
	const TAttribute<FFrameRate> GetTickResolutionAttr = TAttribute<FFrameRate>(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetTickResolution);
	// Create our numeric type interface so we can pass it to the time slider below.
	NumericTypeInterface = MakeShareable(new FFrameNumberInterface(GetDisplayFormatAttr, 0, GetTickResolutionAttr, GetTickResolutionAttr));
	// ** 初始化TimeSlider
	TimeSliderArgs.PlaybackRange = TAttribute<TRange<FFrameNumber>>(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetPlaybackRange);
	TimeSliderArgs.DisplayRate = TAttribute<FFrameRate>(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetTickResolution);
	TimeSliderArgs.TickResolution = TAttribute<FFrameRate>(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetTickResolution);
	TimeSliderArgs.SelectionRange = TAttribute<TRange<FFrameNumber>>(ActActionSequenceEditorRef, &FActActionSequenceEditor::GetSelectionRange);
	TimeSliderArgs.OnPlaybackRangeChanged = ActActionSequence::OnFrameRangeChangedDelegate::CreateSP(ActActionSequenceEditorRef, &FActActionSequenceEditor::SetPlaybackRange);
	TimeSliderArgs.ScrubPosition = TAttribute<FFrameTime>(this, &FActActionSequenceController::GetLocalFrameTime);
	TimeSliderArgs.ScrubPositionText = TAttribute<FString>(this, &FActActionSequenceController::GetFrameTimeText);
	TimeSliderArgs.OnBeginScrubberMovement = FSimpleDelegate::CreateSP(this, &FActActionSequenceController::OnBeginScrubbing);
	TimeSliderArgs.OnEndScrubberMovement = FSimpleDelegate::CreateSP(this, &FActActionSequenceController::OnEndScrubbing);
	TimeSliderArgs.OnScrubPositionChanged = ActActionSequence::OnScrubPositionChangedDelegate::CreateSP(this, &FActActionSequenceController::OnScrubPositionChanged);
	TimeSliderArgs.PlaybackStatus = TAttribute<ActActionSequence::EPlaybackType>(this, &FActActionSequenceController::GetPlaybackStatus);
	TimeSliderArgs.NumericTypeInterface = NumericTypeInterface;
	// ** ViewRange
	TimeSliderArgs.ViewRange.Bind(TAttribute<ActActionSequence::FActActionAnimatedRange>::FGetter::CreateSP(this, &FActActionSequenceController::GetViewRange));
	TimeSliderArgs.OnViewRangeChanged = ActActionSequence::OnViewRangeChangedDelegate::CreateSP(this, &FActActionSequenceController::SetViewRange);
	// ** ClampRange
	TimeSliderArgs.ClampRange.Bind(TAttribute<ActActionSequence::FActActionAnimatedRange>::FGetter::CreateSP(this, &FActActionSequenceController::GetClampRange));
	ActActionTimeSliderController = MakeShareable(new FActActionTimeSliderController(SharedThis(this)));
	ActActionTimeSliderController->MakeTimeSliderWidget();
	ActActionSequenceWidget = SNew(SActActionSequenceWidget, SharedThis(this));
}

TStatId FActActionSequenceController::GetStatId() const
{
	// ** TODO:临时的标记
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

void FActActionSequenceController::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	for (int32 i = 0; i < TrackEditors.Num(); ++i)
	{
		TrackEditors[i]->BuildAddTrackMenu(MenuBuilder);
	}
}

// TODO:这个接口改到Editor中，资源调用Editor接口再调用Controller
void FActActionSequenceController::AddAnimSequenceTrack(UAnimSequence* InAnimSequence)
{
	if (!InAnimSequence)
	{
		UE_LOG(LogActAction, Log, TEXT("FActActionSequenceController::AddAnimMontageTrack with nullptr AnimMontage"))
		return;
	}
	if (ActActionSequenceEditor.IsValid())
	{
		UE_LOG(LogActAction, Log, TEXT("AnimMontage : %s"), *InAnimSequence->GetName());
		const TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = ActActionSequenceEditor.Pin().ToSharedRef();
		ActActionSequenceEditorRef->SetAnimSequence(InAnimSequence);
		if (ActActionTimeSliderController.IsValid())
		{
			const float CalculateSequenceLength = InAnimSequence->GetPlayLength();
			const FFrameRate SamplingFrameRate = InAnimSequence->GetSamplingFrameRate();
			UE_LOG(LogActAction, Log, TEXT("InTotalLength : %f"), CalculateSequenceLength);
			// ** 限制显示的最大长度为当前的Sequence总时长
			// TODO:
			// const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
			// const FFrameTime TargetFrameTime = SamplingFrameRate.AsFrameTime(CalculateSequenceLength);
			// float RealSequenceLength = TargetFrameTime.AsDecimal() / TickResolution.AsDecimal();
			// UE_LOG(LogActAction, Log, TEXT("RealSequenceLength : %f"), RealSequenceLength);
			TargetViewRange = TRange<double>(0, CalculateSequenceLength);
			TargetClampRange = TRange<double>(0, CalculateSequenceLength);
			ActActionTimeSliderController->SetPlaybackRangeEnd(SamplingFrameRate.AsFrameNumber(CalculateSequenceLength));
		}

		if (ActActionSequenceEditor.IsValid())
		{
			ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->InitAnimation(InAnimSequence);
		}
	}
}

void FActActionSequenceController::SetPlaybackStatus(ActActionSequence::EPlaybackType InPlaybackStatus)
{
	PlaybackState = InPlaybackStatus;
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
}

void FActActionSequenceController::Pause()
{
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);
}

FFrameTime FActActionSequenceController::GetLocalFrameTime() const
{
	if (ActActionSequenceEditor.IsValid())
	{
		const TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = ActActionSequenceEditor.Pin().ToSharedRef();
		const FFrameRate TickResolution = ActActionSequenceEditorRef->GetTickResolution();
		const float CurrentPosition = ActActionSequenceEditorRef->GetActActionPreviewSceneController()->GetCurrentPosition();
		return TickResolution.AsFrameTime(CurrentPosition);;
	}
	return FFrameTime(0);
}


FString FActActionSequenceController::GetFrameTimeText() const
{
	const FFrameTime LocalFrameTime = GetLocalFrameTime();
	return NumericTypeInterface->ToString(LocalFrameTime.GetFrame().Value);
}

void FActActionSequenceController::OnBeginScrubbing()
{
	// Pause first since there's no explicit evaluation in the stopped state when OnEndScrubbing() is called
	Pause();
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Scrubbing);
}


void FActActionSequenceController::OnEndScrubbing()
{
	SetPlaybackStatus(ActActionSequence::EPlaybackType::Stopped);
}

void FActActionSequenceController::SetGlobalTime(FFrameTime InFrameTime) const
{
	if (ActActionSequenceEditor.IsValid())
	{
		const TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = ActActionSequenceEditor.Pin().ToSharedRef();
		const FFrameRate TickResolution = ActActionSequenceEditorRef->GetTickResolution();
		// Don't update the sequence if the time hasn't changed as this will cause duplicate events and the like to fire.
		// If we need to reevaluate the sequence at the same time for whatever reason, we should call ForceEvaluate()
		const TSharedRef<FActActionPreviewSceneController> ActActionPreviewSceneController = ActActionSequenceEditorRef->GetActActionPreviewSceneController();
		const float CurrentPosition = ActActionPreviewSceneController->GetCurrentPosition();
		const FFrameTime CurrentFrameTime = TickResolution.AsFrameTime(CurrentPosition);
		if (CurrentFrameTime != InFrameTime)
		{
			const ActActionSequence::FActActionEvaluationRange InRange(InFrameTime, TickResolution);
			ActActionPreviewSceneController->EvaluateInternal(InRange);
		}
	}
}

void FActActionSequenceController::SetLocalTimeDirectly(FFrameTime InFrameTime) const
{
	const TWeakPtr<SWidget> PreviousFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
	// Clear focus before setting time in case there's a key editor value selected that gets committed to a newly selected key on UserMovedFocus
	if (GetPlaybackStatus() == ActActionSequence::EPlaybackType::Stopped)
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}
	// Transform the time to the root time-space
	SetGlobalTime(InFrameTime);
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
	}
	SetLocalTimeDirectly(NewScrubPosition);
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetViewRange() const
{
	ActActionSequence::FActActionAnimatedRange AnimatedRange(TargetViewRange.GetLowerBoundValue(), TargetViewRange.GetUpperBoundValue());
	return AnimatedRange;
}

void FActActionSequenceController::SetViewRange(TRange<double> InViewRange, ActActionSequence::EActActionViewRangeInterpolation InViewRangeInterpolation)
{
	TargetViewRange = InViewRange;
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetClampRange() const
{
	ActActionSequence::FActActionAnimatedRange AnimatedRange(TargetClampRange.GetLowerBoundValue(), TargetClampRange.GetUpperBoundValue());
	return AnimatedRange;
}


void FActActionSequenceController::PopulateAddMenuContext(FMenuBuilder& MenuBuilder)
{
	// ** 填充AddTrack菜单
	MenuBuilder.BeginSection("AddTracks");
	BuildAddTrackMenu(MenuBuilder);
	MenuBuilder.EndSection();
}
#undef LOCTEXT_NAMESPACE
