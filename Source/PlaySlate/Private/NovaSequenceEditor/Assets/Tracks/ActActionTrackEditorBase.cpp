#include "ActActionTrackEditorBase.h"

#include "PlaySlate.h"

FActActionTrackEditorBase::FActActionTrackEditorBase(const TSharedRef<FActActionSequenceController>& InSequenceController)
	: SequenceController(InSequenceController)
{
}

FActActionTrackEditorBase::~FActActionTrackEditorBase()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionTrackEditorBase::~FActActionTrackEditorBase"));
}
