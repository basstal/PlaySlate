#include "ActEventTimeline.h"

#include "PlaySlate.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/Assets/Tracks/ActActionTrackEditorBase.h"
#include "NovaAct/ActEventTimeline/ActEventTimelineSlider.h"
#include "NovaAct/ActViewport/ActViewport.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionSequenceTreeViewNode.h"
#include "NovaAct/Widgets/ActEventTimeline/ActActionSequenceWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"

#include "IContentBrowserSingleton.h"
#include "LevelEditorViewport.h"
#include "Animation/AnimInstance.h"
#include "Animation/DebugSkelMeshComponent.h"
#include "AnimPreviewInstance.h"
#include "FrameNumberNumericInterface.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/Tracks/ActActionHitBoxTrack.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActEventTimeline::FActEventTimeline(const TSharedRef<FNovaActEditor>& InNovaActEditor)
	: NovaActEditor(InNovaActEditor)
{
	// ** 将Sequence能编辑的所有TrackEditor注册，以便能够使用AddTrackEditor以及AddTrackMenu
	TrackEditorDelegates.Add(OnCreateTrackEditorDelegate::CreateStatic(FActActionHitBoxTrack::CreateTrackEditor));
}

FActEventTimeline::~FActEventTimeline()
{
	UE_LOG(LogActAction, Log, TEXT("FActEventTimeline::~FActEventTimeline"));
	ActActionSequenceWidget.Reset();
	ActActionTimeSliderController.Reset();
	ActActionSequenceTreeViewNode.Reset();
	TrackEditors.Empty();
	NovaDB::Delete<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	NovaDB::Delete<TSharedPtr<FActEventTimelineEvents>>("ActEventTimelineEvents");
	ActEventTimelineArgsDB.Reset();
	ActEventTimelineEventsDB.Reset();
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	ActAnimationDB->UnBind(OnHitBoxesChangedHandle);
	ActAnimationDB->UnBind(OnAnimSequenceChangedHandle);
}


void FActEventTimeline::Init()
{
	// ** EventTimeline 共享参数的初始化
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = MakeShareable(new FActEventTimelineArgs());
	auto TickResolutionAttrLambda = MakeAttributeLambda([]()
	{
		auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
		return ActEventTimelineArgs->TickResolution;
	});
	ActEventTimelineArgs->NumericTypeInterface = MakeShareable(new FFrameNumberInterface(MakeAttributeLambda([]()
	                                                                                     {
		                                                                                     return EFrameNumberDisplayFormats::Frames;
	                                                                                     }),
	                                                                                     0,
	                                                                                     TickResolutionAttrLambda,
	                                                                                     TickResolutionAttrLambda));
	ActEventTimelineArgsDB = StaticCastSharedPtr<TDataBindingSP<FActEventTimelineArgs>>(NovaDB::CreateSP("ActEventTimelineArgs", ActEventTimelineArgs));
	// ** EventTimeline 事件的初始化
	TSharedPtr<FActEventTimelineEvents> ActEventTimelineEventsPtr = MakeShareable(new FActEventTimelineEvents());
	ActEventTimelineEventsPtr->OnBeginScrubberMovement = FSimpleDelegate::CreateSP(this, &FActEventTimeline::OnBeginScrubberMovement);
	ActEventTimelineEventsPtr->OnEndScrubberMovement = FSimpleDelegate::CreateSP(this, &FActEventTimeline::OnEndScrubberMovement);
	ActEventTimelineEventsPtr->OnScrubPositionChanged = OnScrubPositionChangedDelegate::CreateSP(this, &FActEventTimeline::OnScrubPositionChanged);
	ActEventTimelineEventsDB = StaticCastSharedPtr<TDataBindingSP<FActEventTimelineEvents>>(NovaDB::CreateSP("ActEventTimelineEvents", ActEventTimelineEventsPtr));

	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	OnHitBoxesChangedHandle = ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &FActEventTimeline::OnHitBoxesChanged));
	OnAnimSequenceChangedHandle = ActAnimationDB->Bind(TDataBindingUObject<UActAnimation>::DelegateType::CreateRaw(this, &FActEventTimeline::OnAnimSequenceChanged));

	// ** 构造所有显示节点的根节点
	ActActionSequenceTreeViewNode = MakeShareable(new FActActionSequenceTreeViewNode(SharedThis(this)));

	ActActionTimeSliderController = MakeShareable(new FActEventTimelineSlider(SharedThis(this)));
	ActActionTimeSliderController->Init();
	ActActionSequenceWidget = SNew(SActActionSequenceWidget, SharedThis(this));

	// 调用已注册的TrackEditor的Create代理，并收集创建的TrackEditor实例
	for (int32 DelegateIndex = 0; DelegateIndex < TrackEditorDelegates.Num(); ++DelegateIndex)
	{
		check(TrackEditorDelegates[DelegateIndex].IsBound());
		// Tools may exist in other modules, call a delegate that will create one for us 
		TSharedRef<FActActionTrackEditorBase> TrackEditor = TrackEditorDelegates[DelegateIndex].Execute(SharedThis(this));
		TrackEditors.Add(TrackEditor);
	}
}

void FActEventTimeline::Tick(float DeltaTime)
{}

TStatId FActEventTimeline::GetStatId() const
{
	// ** TODO:临时的标记
	RETURN_QUICK_DECLARE_CYCLE_STAT(FActActionSequenceController, STATGROUP_Tickables);
}

void FActEventTimeline::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	for (int32 i = 0; i < TrackEditors.Num(); ++i)
	{
		TrackEditors[i]->BuildAddTrackMenu(MenuBuilder);
	}
}

// TODO:这个接口改到Editor中，资源调用Editor接口再调用Controller
void FActEventTimeline::OnAnimSequenceChanged(UActAnimation* InActAnimation)
{
	if (!InActAnimation)
	{
		return;
	}
	UAnimSequence* InAnimSequence = InActAnimation->AnimSequence;
	if (!InAnimSequence)
	{
		UE_LOG(LogActAction, Log, TEXT("FActEventTimeline::AddAnimMontageTrack with nullptr AnimMontage"))
		return;
	}
	UE_LOG(LogActAction, Log, TEXT("AnimMontage : %s"), *InAnimSequence->GetName());
	if (ActActionTimeSliderController.IsValid())
	{
		const float CalculateSequenceLength = InAnimSequence->GetPlayLength();
		// const FFrameRate SamplingFrameRate = InAnimSequence->GetSamplingFrameRate();
		UE_LOG(LogActAction, Log, TEXT("InTotalLength : %f"), CalculateSequenceLength);
		// ** 限制显示的最大长度为当前的Sequence总时长
		auto ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
		ActEventTimelineArgs->ViewRange = TRange<float>(0, CalculateSequenceLength);
		ActEventTimelineArgs->ClampRange = TRange<float>(0, CalculateSequenceLength);
		ActEventTimelineArgs->TickResolution = InAnimSequence->GetSamplingFrameRate();;
		// ActActionTimeSliderController->SetPlaybackRangeEnd(SamplingFrameRate.AsFrameNumber(CalculateSequenceLength));
	}
}

void FActEventTimeline::SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus)
{
	ActEventTimelineArgsDB->GetData()->PlaybackStatus = InPlaybackStatus;
	ESequencerState State = ESS_None;
	switch (InPlaybackStatus)
	{
	case ENovaPlaybackType::Playing:
	case ENovaPlaybackType::Recording:
		{
			State = ESS_Playing;
			break;
		}
	case ENovaPlaybackType::Stopped:
	case ENovaPlaybackType::Scrubbing:
	case ENovaPlaybackType::Stepping:
		{
			State = ESS_Paused;
			break;
		}
	default: ;
	}
	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC && LevelVC->AllowsCinematicControl())
		{
			LevelVC->ViewState.GetReference()->SetSequencerState(State);
		}
	}
}

// void FActEventTimeline::Pause()
// {
// }

// FFrameTime FActEventTimeline::GetLocalFrameTime() const
// {
// 	if (NovaActEditor.IsValid())
// 	{
// 		const TSharedRef<FNovaActEditor> ActActionSequenceEditorRef = NovaActEditor.Pin().ToSharedRef();
// 		const FFrameRate TickResolution = ActActionSequenceEditorRef->GetTickResolution();
// 		const float CurrentPosition = ActActionSequenceEditorRef->GetActActionPreviewSceneController()->GetCurrentPosition();
// 		return TickResolution.AsFrameTime(CurrentPosition);
// 	}
// 	return FFrameTime(0);
// }


// FString FActEventTimeline::GetFrameTimeText() const
// {
// 	const FFrameTime LocalFrameTime = GetLocalFrameTime();
// 	return NumericTypeInterface->ToString(LocalFrameTime.GetFrame().Value);
// }

void FActEventTimeline::OnBeginScrubberMovement()
{
	// Pause first since there's no explicit evaluation in the stopped state when OnEndScrubberMovement() is called
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
	SetPlaybackStatus(ENovaPlaybackType::Scrubbing);
}


void FActEventTimeline::OnEndScrubberMovement()
{
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
}

// void FActEventTimeline::SetGlobalTime(FFrameTime InFrameTime) const
// {
// 	if (NovaActEditor.IsValid())
// 	{
// 		const TSharedRef<FNovaActEditor> ActActionSequenceEditorRef = NovaActEditor.Pin().ToSharedRef();
// 		const FFrameRate TickResolution = ActActionSequenceEditorRef->GetTickResolution();
// 		// Don't update the sequence if the time hasn't changed as this will cause duplicate events and the like to fire.
// 		// If we need to reevaluate the sequence at the same time for whatever reason, we should call ForceEvaluate()
// 		const TSharedRef<FActActionPreviewSceneController> ActActionPreviewSceneController = ActActionSequenceEditorRef->GetActActionPreviewSceneController();
// 		const float CurrentPosition = ActActionPreviewSceneController->GetCurrentPosition();
// 		const FFrameTime CurrentFrameTime = TickResolution.AsFrameTime(CurrentPosition);
// 		if (CurrentFrameTime != InFrameTime)
// 		{
// 			const FActActionEvaluationRange InRange(InFrameTime, TickResolution);
// 			ActActionPreviewSceneController->OnCurrentTimeChanged(InRange);
// 		}
// 	}
// }

// void FActEventTimeline::SetLocalTimeDirectly(FFrameTime InFrameTime) const
// { }

void FActEventTimeline::OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	if (ActEventTimelineArgs->PlaybackStatus == ENovaPlaybackType::Scrubbing && !bScrubbing)
	{
		OnEndScrubberMovement();
	}
	const TWeakPtr<SWidget> PreviousFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
	// Clear focus before setting time in case there's a key editor value selected that gets committed to a newly selected key on UserMovedFocus
	if (ActEventTimelineArgs->PlaybackStatus == ENovaPlaybackType::Stopped)
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}
	if (NewScrubPosition != ActEventTimelineArgs->CurrentTime)
	{
		ActEventTimelineArgs->CurrentTime = NewScrubPosition;
		ActEventTimelineArgsDB->Trigger();
	}
	if (PreviousFocusedWidget.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(PreviousFocusedWidget.Pin());
	}
}

// TRange<float> FActEventTimeline::GetViewRange() const
// {
// 	TRange<float> AnimatedRange(TargetViewRange.GetLowerBoundValue(), TargetViewRange.GetUpperBoundValue());
// 	return AnimatedRange;
// }
//
// void FActEventTimeline::SetViewRange(TRange<float> InViewRange, ENovaViewRangeInterpolation InViewRangeInterpolation)
// {
// 	TargetViewRange = InViewRange;
// }

// TRange<float> FActEventTimeline::GetClampRange() const
// {
// 	TRange<float> AnimatedRange(TargetClampRange.GetLowerBoundValue(), TargetClampRange.GetUpperBoundValue());
// 	return AnimatedRange;
// }


void FActEventTimeline::PopulateAddMenuContext(FMenuBuilder& MenuBuilder)
{
	// ** 填充AddTrack菜单
	MenuBuilder.BeginSection("AddTracks");
	BuildAddTrackMenu(MenuBuilder);
	MenuBuilder.EndSection();
}

void FActEventTimeline::OnHitBoxesChanged(UActAnimation* InActAnimation)
{
	TArray<FActActionHitBoxData>& InHitBoxData = InActAnimation->ActActionHitBoxes;
	const TSharedRef<FActActionSequenceTreeViewNode> HitBoxesFolder = ActActionSequenceTreeViewNode->FindOrCreateFolder(FName("HitBoxesFolder"));
	int HitBoxTreeViewNodeCount = HitBoxesFolder->GetChildNodes().Num();
	if (HitBoxTreeViewNodeCount < InHitBoxData.Num())
	{
		for (int count = HitBoxTreeViewNodeCount; count < InHitBoxData.Num(); ++count)
		{
			TSharedRef<FActActionSequenceTreeViewNode> NewTreeViewNode = MakeShareable(new FActActionSequenceTreeViewNode(SharedThis(this), "HitBox", ENovaSequenceNodeType::State));
			NewTreeViewNode->SetParent(HitBoxesFolder);
		}
	}
	int Index = 0;
	for (FActActionHitBoxData& InHitBox : InHitBoxData)
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetContentAsHitBox(InHitBox);
	}
	while (Index < HitBoxesFolder->GetChildNodes().Num())
	{
		HitBoxesFolder->GetChildByIndex(Index++)->SetVisible(EVisibility::Collapsed);
	}
	ActActionSequenceTreeViewNode->Refresh();
}
#undef LOCTEXT_NAMESPACE
