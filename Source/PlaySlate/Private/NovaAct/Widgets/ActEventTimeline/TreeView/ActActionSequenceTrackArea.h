#pragma once

#include "NovaAct/ActEventTimeline/TreeView/ActActionTrackAreaSlot.h"

class SActActionSequenceTrackLane;
class FActActionSequenceTreeViewNode;


class SActActionSequenceTrackArea : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTrackArea) { }

	SLATE_END_ARGS()

	SActActionSequenceTrackArea();

	void Construct(const FArguments& InArgs);
	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

	TSharedPtr<SActActionSequenceTrackLane> FindTrackSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InNode) const;

	/** Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed. */
	TSharedRef<SActActionSequenceTrackLane> AddTrackSlot(const TSharedRef<FActActionSequenceTreeViewNode>& InNode);
protected:
	/** The track area's children. */
	TPanelChildren<FActActionTrackAreaSlot> Children;
	/** A map of child slot content that exist in our view. */
	TMap<TSharedPtr<FActActionSequenceTreeViewNode>, TWeakPtr<SActActionSequenceTrackLane>> TrackSlots;

};
