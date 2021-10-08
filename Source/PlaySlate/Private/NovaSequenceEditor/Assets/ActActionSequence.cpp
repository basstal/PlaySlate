#include "ActActionSequence.h"

UActActionSequence::UActActionSequence()
	: TickResolution(60, 1),
	  DisplayRate(60, 1),
	  PlaybackRange(TRange<FFrameNumber>::Inclusive(0, 60)),
	  SelectionRange(TRange<FFrameNumber>::Inclusive(0, 60))
{
}
