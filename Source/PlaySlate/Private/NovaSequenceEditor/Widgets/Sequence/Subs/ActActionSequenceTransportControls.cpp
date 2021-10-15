﻿#include "ActActionSequenceTransportControls.h"

#include "PlaySlate.h"
#include "EditorWidgetsModule.h"
#include "ITransportControl.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"

SActActionSequenceTransportControls::~SActActionSequenceTransportControls()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceTransportControls::~SActActionSequenceTransportControls"));
}

void SActActionSequenceTransportControls::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InActActionSequenceController)
{
	ActActionSequenceController = InActActionSequenceController;
	ActActionSequenceEditor = ActActionSequenceController.Pin()->GetActActionSequenceEditor();

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
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->PlayStep(true);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Forward_End()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->EvaluateToOneEnd(true);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward_Step()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->PlayStep(false);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward_End()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->EvaluateToOneEnd(false);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Forward()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->TogglePlay(true);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_Backward()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->TogglePlay(false);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClick_ToggleLoop()
{
	ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->ToggleLoop();
	return FReply::Handled();
}

bool SActActionSequenceTransportControls::IsLoopStatusOn() const
{
	return ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->IsLoopStatusOn();
}

EPlaybackMode::Type SActActionSequenceTransportControls::GetPlaybackMode() const
{
	return ActActionSequenceEditor.Pin()->GetActActionPreviewSceneController()->GetPlaybackMode();
}
