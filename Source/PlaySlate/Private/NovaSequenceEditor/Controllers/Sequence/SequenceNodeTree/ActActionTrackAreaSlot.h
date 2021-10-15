#pragma once

class SActActionSequenceTrackLane;

/**
* Structure representing a slot in the track area.
*/
class FActActionTrackAreaSlot : public TSlotBase<FActActionTrackAreaSlot>
{
public:
	/** Construction from a track lane */
	FActActionTrackAreaSlot(const TSharedRef<SActActionSequenceTrackLane>& InSlotContent);

	/** Get the vertical position of this slot inside its parent. */
	float GetVerticalOffset() const;

protected:
	/** The track lane that we represent. */
	TWeakPtr<SActActionSequenceTrackLane> TrackLane;

	/** Horizontal alignment for the slot. */
	EHorizontalAlignment HAlignment;
	/** Vertical alignment for the slot. */
	EVerticalAlignment VAlignment;
};
