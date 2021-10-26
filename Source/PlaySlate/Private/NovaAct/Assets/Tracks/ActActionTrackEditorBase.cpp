#include "ActActionTrackEditorBase.h"

#include "PlaySlate.h"

FActActionTrackEditorBase::FActActionTrackEditorBase(const TSharedRef<SActEventTimelineWidget>& InActEventTimelineWidget)
	: ActEventTimelineWidget(InActEventTimelineWidget)
{
}

FActActionTrackEditorBase::~FActActionTrackEditorBase()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActActionTrackEditorBase::~FActActionTrackEditorBase"));
}
