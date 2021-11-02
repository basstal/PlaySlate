#pragma once

#include "ActImageTrackLaneWidget.h"

class SActImageAreaPanel : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActImageAreaPanel) { }
	SLATE_END_ARGS()

	SActImageAreaPanel();
	virtual ~SActImageAreaPanel() override;

	void Construct(const FArguments& InArgs);

	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

	/**
	 * Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed.
	 *
	 * @param InActImageTreeViewTableRow 
	 * @return
	 */
	TSharedRef<SActImageTrackLaneWidget> MakeTrackLane(const TSharedRef<SActImageTreeViewTableRow>& InActImageTreeViewTableRow);

protected:
	TPanelChildren<SActImageTrackLaneWidget::Slot> Children;// ** The track area's children.
};
