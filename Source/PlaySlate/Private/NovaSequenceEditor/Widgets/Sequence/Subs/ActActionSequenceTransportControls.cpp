#include "ActActionSequenceTransportControls.h"

#include "PlaySlate.h"
#include "EditorWidgetsModule.h"
#include "ITransportControl.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"

SActActionSequenceTransportControls::~SActActionSequenceTransportControls()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceTransportControls::~SActActionSequenceTransportControls"));
}

void SActActionSequenceTransportControls::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InActActionSequenceController)
{
	ActActionPreviewSceneController = InActActionSequenceController->GetActActionSequenceEditor()->GetActActionPreviewSceneController();

	FEditorWidgetsModule& EditorWidgetsModule = FModuleManager::LoadModuleChecked<FEditorWidgetsModule>("EditorWidgets");

	FTransportControlArgs TransportControlArgs;
	TransportControlArgs.OnForwardPlay = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Forward);
	TransportControlArgs.OnBackwardPlay = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Backward);
	TransportControlArgs.OnForwardStep = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Forward_Step);
	TransportControlArgs.OnBackwardStep = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Backward_Step);
	TransportControlArgs.OnForwardEnd = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Forward_End);
	TransportControlArgs.OnBackwardEnd = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_Backward_End);
	TransportControlArgs.OnToggleLooping = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClick_ToggleLoop);
	TransportControlArgs.OnGetLooping = FOnGetLooping::CreateSP(this, &SActActionSequenceTransportControls::IsLoopStatusOn);
	TransportControlArgs.OnGetPlaybackMode = FOnGetPlaybackMode::CreateSP(this, &SActActionSequenceTransportControls::GetPlaybackMode);

	ChildSlot
	[
		EditorWidgetsModule.CreateTransportControl(TransportControlArgs)
	];
}

FReply SActActionSequenceTransportControls::OnClick_Forward_Step()
{
	ActActionPreviewSceneController.Pin()->PlayStep(true);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Forward_End()
{
	ActActionPreviewSceneController.Pin()->EvaluateToOneEnd(true);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward_Step()
{
	ActActionPreviewSceneController.Pin()->PlayStep(false);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward_End()
{
	ActActionPreviewSceneController.Pin()->EvaluateToOneEnd(false);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Forward()
{
	TSharedRef<FActActionPreviewSceneController> ActActionPreviewSceneControllerRef = ActActionPreviewSceneController.Pin().ToSharedRef();
	EPlaybackMode::Type PlaybackMode = ActActionPreviewSceneControllerRef->GetPlaybackMode();
	if (PlaybackMode == EPlaybackMode::Stopped || PlaybackMode == EPlaybackMode::PlayingReverse)
	{
		ActActionPreviewSceneControllerRef->TogglePlay(EPlaybackMode::PlayingForward);
	}
	else
	{
		ActActionPreviewSceneControllerRef->TogglePlay(EPlaybackMode::Stopped);
	}
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward()
{
	TSharedRef<FActActionPreviewSceneController> ActActionPreviewSceneControllerRef = ActActionPreviewSceneController.Pin().ToSharedRef();
	EPlaybackMode::Type PlaybackMode = ActActionPreviewSceneControllerRef->GetPlaybackMode();
	if (PlaybackMode == EPlaybackMode::Stopped || PlaybackMode == EPlaybackMode::PlayingForward)
	{
		ActActionPreviewSceneControllerRef->TogglePlay(EPlaybackMode::PlayingReverse);
	}
	else
	{
		ActActionPreviewSceneControllerRef->TogglePlay(EPlaybackMode::Stopped);
	}
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_ToggleLoop()
{
	ActActionPreviewSceneController.Pin()->ToggleLoop();
	return FReply::Handled();
}

bool SActActionSequenceTransportControls::IsLoopStatusOn() const
{
	return ActActionPreviewSceneController.Pin()->IsLoopStatusOn();
}

EPlaybackMode::Type SActActionSequenceTransportControls::GetPlaybackMode() const
{
	return ActActionPreviewSceneController.Pin()->GetPlaybackMode();
}
