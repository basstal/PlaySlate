#include "ActActionSequence.h"

UActActionSequence::UActActionSequence()
	: TickResolution(),
	  DisplayRate(),
	  PlaybackRange(TRange<FFrameNumber>::Inclusive(0, 60)),
	  SelectionRange(TRange<FFrameNumber>::Inclusive(0, 60))
{
}
