#include "ActActionSequenceController.h"

#include "IContentBrowserSingleton.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "PlaySlate/PlaySlate.h"
#include "SWidget/ActActionSequenceWidget.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionSequenceController::FActActionSequenceController(UActActionSequence* InActActionSequence)
	: ActActionSequencePtr(InActActionSequence),
	  TargetViewRange(0.f, 5.f),
	  LastViewRange(0.f, 5.f),
	  PlaybackState(),
	  PreviewActor(nullptr),
	  NodeTree(nullptr)
{
}

FActActionSequenceController::~FActActionSequenceController()
{
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

void FActActionSequenceController::AssignAsEditAnim(ASkeletalMeshActor* InPreviewActor)
{
	if (InPreviewActor)
	{
		UE_LOG(LogActAction, Log, TEXT("AssignAsEditAnim PreviewActor : %s"), *InPreviewActor->GetName());
		ActActionSequencePtr->EditAnimInstance = InPreviewActor->GetSkeletalMeshComponent()->GetAnimInstance();
		PreviewActor = InPreviewActor;
		if (ActActionViewportWidget.IsValid() && ActActionViewportWidget->GetPreviewScenePtr().IsValid())
		{
			ActActionViewportWidget->GetPreviewScenePtr()->InitPreviewScene(InPreviewActor);
		}
	}
}

ActActionSequence::FActActionAnimatedRange FActActionSequenceController::GetViewRange() const
{
	float LowerRange = FMath::Lerp(LastViewRange.GetLowerBoundValue(), TargetViewRange.GetLowerBoundValue(), ZoomCurve.GetLerp());
	float HigherRange = FMath::Lerp(LastViewRange.GetUpperBoundValue(), TargetViewRange.GetUpperBoundValue(), ZoomCurve.GetLerp());
	ActActionSequence::FActActionAnimatedRange AnimatedRange(LowerRange, HigherRange);
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

TSharedPtr<SActActionSequenceWidget> FActActionSequenceController::MakeSequenceWidget(ActActionSequence::FActActionSequenceViewParams ViewParams)
{
	return SNew(SActActionSequenceWidget, SharedThis(this))
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
		// .ScrubPosition(this, &FActActionSequenceController::GetLocalFrameTime)
		// .ScrubPositionText(this, &FActActionSequenceController::GetFrameTimeText)
		// .ScrubPositionParent(this, &FActActionSequenceController::GetScrubPositionParent)
		// .ScrubPositionParentChain(this, &FActActionSequenceController::GetScrubPositionParentChain)
		// .OnScrubPositionParentChanged(this, &FActActionSequenceController::OnScrubPositionParentChanged)
		// .OnBeginScrubbing(this, &FActActionSequenceController::OnBeginScrubbing)
		// .OnEndScrubbing(this, &FActActionSequenceController::OnEndScrubbing)
		// .OnScrubPositionChanged(this, &FActActionSequenceController::OnScrubPositionChanged)
		// .OnViewRangeChanged(this, &FActActionSequenceController::SetViewRange)
		// .OnClampRangeChanged(this, &FActActionSequenceController::OnClampRangeChanged)
		// .OnGetNearestKey(this, &FActActionSequenceController::OnGetNearestKey)
		.OnGetAddMenuContent(ViewParams.OnGetAddMenuContent)
		// .OnBuildCustomContextMenuForGuid(ViewParams.OnBuildCustomContextMenuForGuid)
		.OnReceivedFocus(ViewParams.OnReceivedFocus)
		.AddMenuExtender(ViewParams.AddMenuExtender)
		.ToolbarExtender(ViewParams.ToolbarExtender);
}


#undef LOCTEXT_NAMESPACE
