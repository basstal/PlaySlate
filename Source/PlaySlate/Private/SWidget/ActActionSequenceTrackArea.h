#pragma once

/**
* Structure representing a slot in the track area.
*/
class FActActionTrackAreaSlot : public TSlotBase<FActActionTrackAreaSlot>
{
public:
	/** Construction from a track lane */
	FActActionTrackAreaSlot();

	/** Get the vertical position of this slot inside its parent. */
	// float GetVerticalOffset() const;

	/** Horizontal/Vertical alignment for the slot. */
	// EHorizontalAlignment HAlignment;
	// EVerticalAlignment VAlignment;

	/** The track lane that we represent. */
	// TWeakPtr<SSequencerTrackLane> TrackLane;
};

class SActActionSequenceTrackArea : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTrackArea)
		{
		}

	SLATE_END_ARGS()

	SActActionSequenceTrackArea();

	void Construct(const FArguments& InArgs);
	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

protected:
	/** The track area's children. */
	TPanelChildren<FActActionTrackAreaSlot> Children;
};
