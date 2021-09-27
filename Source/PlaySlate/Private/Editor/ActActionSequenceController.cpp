#include "ActActionSequenceController.h"

#include "IContentBrowserSingleton.h"
#include "Animation/AnimBlueprint.h"
#include "Animation/AnimInstance.h"
#include "Animation/SkeletalMeshActor.h"
#include "Chaos/AABB.h"
#include "Chaos/AABB.h"
#include "PlaySlate/PlaySlate.h"
#include "SWidget/ActActionSequenceWidget.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionSequenceController::FActActionSequenceController(UActActionSequence* InActActionSequence)
	: ActActionSequencePtr(InActActionSequence),
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
	ActActionSequence::FActActionAnimatedRange AnimatedRange(FMath::Lerp(LastViewRange.GetLowerBoundValue(), TargetViewRange.GetLowerBoundValue(), ZoomCurve.GetLerp()),
	                                                         FMath::Lerp(LastViewRange.GetUpperBoundValue(), TargetViewRange.GetUpperBoundValue(), ZoomCurve.GetLerp()));

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
		SequenceWidget->GetTreeView()->AddRootNodes(SequenceDisplayNode);
	}
}


#undef LOCTEXT_NAMESPACE
