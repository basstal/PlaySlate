#include "ActActionTrackEditorBase.h"

#include "PlaySlate.h"

FActActionTrackEditorBase::FActActionTrackEditorBase(const TSharedRef<FActEventTimelineBrain>& InSequenceController)
	: ActActionSequenceController(InSequenceController)
{
}

FActActionTrackEditorBase::~FActActionTrackEditorBase()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionTrackEditorBase::~FActActionTrackEditorBase"));
}
