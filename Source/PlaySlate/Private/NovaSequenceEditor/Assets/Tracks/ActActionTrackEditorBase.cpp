#include "ActActionTrackEditorBase.h"

FActActionTrackEditorBase::FActActionTrackEditorBase(const TSharedRef<FActActionSequenceController>& InSequenceController)
{
	SequenceController = InSequenceController;
}

FActActionTrackEditorBase::~FActActionTrackEditorBase()
{
}
