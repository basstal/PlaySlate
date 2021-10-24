#include "ActActionSequenceTransportControls.h"

#include "PlaySlate.h"
#include "EditorWidgetsModule.h"
#include "ITransportControl.h"
#include "NovaAct/NovaActEditor.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
#include "NovaAct/ActViewport/ActViewport.h"

SActActionSequenceTransportControls::~SActActionSequenceTransportControls()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionSequenceTransportControls::~SActActionSequenceTransportControls"));
}

void SActActionSequenceTransportControls::Construct(const FArguments& InArgs, const TSharedRef<FActEventTimeline>& InActActionSequenceController)
{
	ActActionSequenceEditor = InActActionSequenceController->GetActActionSequenceEditor();

	FEditorWidgetsModule& EditorWidgetsModule = FModuleManager::LoadModuleChecked<FEditorWidgetsModule>("EditorWidgets");

	FTransportControlArgs TransportControlArgs;
	TransportControlArgs.OnForwardPlay = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickForwardPlay);
	TransportControlArgs.OnBackwardPlay = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickBackwardPlay);
	TransportControlArgs.OnForwardStep = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickForwardStep);
	TransportControlArgs.OnBackwardStep = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickBackwardStep);
	TransportControlArgs.OnForwardEnd = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickForwardEnd);
	TransportControlArgs.OnBackwardEnd = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickBackwardEnd);
	TransportControlArgs.OnToggleLooping = FOnClicked::CreateSP(this, &SActActionSequenceTransportControls::OnClickToggleLooping);
	TransportControlArgs.OnGetLooping = FOnGetLooping::CreateSP(this, &SActActionSequenceTransportControls::IsLoopStatusOn);
	TransportControlArgs.OnGetPlaybackMode = FOnGetPlaybackMode::CreateSP(this, &SActActionSequenceTransportControls::GetPlaybackMode);

	ChildSlot
	[
		EditorWidgetsModule.CreateTransportControl(TransportControlArgs)
	];
}

FReply SActActionSequenceTransportControls::OnClickForwardStep() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardStep);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickForwardEnd() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardEnd);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickBackwardStep() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardStep);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickBackwardEnd() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardEnd);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickForwardPlay() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardPlay);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickBackwardPlay() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardPlay);
	return FReply::Handled();
}

FReply SActActionSequenceTransportControls::OnClickToggleLooping() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ToggleLooping);
	return FReply::Handled();
}

bool SActActionSequenceTransportControls::IsLoopStatusOn() const
{
	auto DB = GetDataBinding(bool, "PreviewInstanceLooping");
	return DB->GetData();
}

EPlaybackMode::Type SActActionSequenceTransportControls::GetPlaybackMode() const
{
	auto DB = GetDataBinding(EPlaybackMode::Type, "PreviewInstancePlaybackMode");
	return DB->GetData();
}
