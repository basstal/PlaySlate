#include "ActActionTrackAreaSlot.h"


FActActionTrackAreaSlot::FActActionTrackAreaSlot(const TSharedRef<SActActionSequenceTrackLane>& InSlotContent)
	: TrackLane(InSlotContent),
	  HAlignment(),
	  VAlignment()
{
}

float FActActionTrackAreaSlot::GetVerticalOffset() const
{
	// ** TODO:
	return 0.0f;
}
