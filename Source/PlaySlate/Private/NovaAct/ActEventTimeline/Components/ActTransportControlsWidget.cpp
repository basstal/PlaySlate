#include "ActTransportControlsWidget.h"

#include "PlaySlate.h"
#include "EditorWidgetsModule.h"
#include "ITransportControl.h"

#include "NovaAct/ActViewport/ActViewport.h"

SActTransportControlsWidget::~SActTransportControlsWidget()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActTransportControlsWidget::~SActTransportControlsWidget"));
}

void SActTransportControlsWidget::Construct(const FArguments& InArgs)
{
	FEditorWidgetsModule& EditorWidgetsModule = FModuleManager::LoadModuleChecked<FEditorWidgetsModule>("EditorWidgets");

	FTransportControlArgs TransportControlArgs;
	{
		TransportControlArgs.OnForwardPlay = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickForwardPlay);
		TransportControlArgs.OnBackwardPlay = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickBackwardPlay);
		TransportControlArgs.OnForwardStep = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickForwardStep);
		TransportControlArgs.OnBackwardStep = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickBackwardStep);
		TransportControlArgs.OnForwardEnd = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickForwardEnd);
		TransportControlArgs.OnBackwardEnd = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickBackwardEnd);
		TransportControlArgs.OnToggleLooping = FOnClicked::CreateSP(this, &SActTransportControlsWidget::OnClickToggleLooping);
		TransportControlArgs.OnGetLooping = FOnGetLooping::CreateSP(this, &SActTransportControlsWidget::IsLoopStatusOn);
		TransportControlArgs.OnGetPlaybackMode = FOnGetPlaybackMode::CreateSP(this, &SActTransportControlsWidget::GetPlaybackMode);
	}

	ChildSlot
	[
		EditorWidgetsModule.CreateTransportControl(TransportControlArgs)
	];
}

FReply SActTransportControlsWidget::OnClickForwardStep() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardStep);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickForwardEnd() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardEnd);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickBackwardStep() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardStep);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickBackwardEnd() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardEnd);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickForwardPlay() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ForwardPlay);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickBackwardPlay() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::BackwardPlay);
	return FReply::Handled();
}

FReply SActTransportControlsWidget::OnClickToggleLooping() const
{
	auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
	DB->SetData(ENovaTransportControls::ToggleLooping);
	return FReply::Handled();
}

bool SActTransportControlsWidget::IsLoopStatusOn() const
{
	auto DB = GetDataBinding(bool, "PreviewInstanceLooping");
	return DB->GetData();
}

EPlaybackMode::Type SActTransportControlsWidget::GetPlaybackMode() const
{
	auto DB = GetDataBinding(EPlaybackMode::Type, "PreviewInstancePlaybackMode");
	return DB->GetData();
}
