#pragma once

#include "Editor/ActActionSequenceDisplayNode.h"

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

	/** Horizontal/Vertical alignment for the slot. */
	EHorizontalAlignment HAlignment;
	EVerticalAlignment VAlignment;

	/** The track lane that we represent. */
	TWeakPtr<SActActionSequenceTrackLane> TrackLane;
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

	TSharedPtr<SActActionSequenceTrackLane> FindTrackSlot(const TSharedRef<FActActionSequenceDisplayNode>& InNode);

	/** Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed. */
	void AddTrackSlot(const TSharedRef<FActActionSequenceDisplayNode>& InNode, const TSharedRef<SActActionSequenceTrackLane>& InSlot);
protected:
	/** The track area's children. */
	TPanelChildren<FActActionTrackAreaSlot> Children;
	/** A map of child slot content that exist in our view. */
	TMap<TSharedPtr<FActActionSequenceDisplayNode>, TWeakPtr<SActActionSequenceTrackLane>> TrackSlots;
};
