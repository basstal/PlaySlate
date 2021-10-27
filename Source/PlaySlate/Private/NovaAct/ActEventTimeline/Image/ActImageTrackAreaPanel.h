#pragma once

#include "ActImageTrackLaneWidget.h"

class SActImageTrackAreaPanel : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActImageTrackAreaPanel) { }
	SLATE_END_ARGS()

	SActImageTrackAreaPanel();
	virtual ~SActImageTrackAreaPanel() override;

	void Construct(const FArguments& InArgs);

	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

	/**
	 * Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed.
	 *
	 * @return
	 */
	TSharedRef<SActImageTrackLaneWidget> MakeTrackLane();

protected:
	TPanelChildren<SActImageTrackLaneWidget::Slot> Children;// ** The track area's children.
};
