#pragma once

#include "ActImagePoolWidget.h"

class IActImageTrackBase;

class SActImagePoolAreaPanel : public SPanel
{
public:
	SLATE_BEGIN_ARGS(SActImagePoolAreaPanel) { }
	SLATE_END_ARGS()

	SActImagePoolAreaPanel();
	virtual ~SActImagePoolAreaPanel() override;

	void Construct(const FArguments& InArgs);

	//~Begin SPanel interface
	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual FChildren* GetChildren() override;
	//~End SPanel interface

	/**
	 * Add a new track slot to this area for the given node. The slot will be automatically cleaned up when all external references to the supplied slot are removed.
	 *
	 * @param InActImageTrack 
	 * @return
	 */
	TSharedRef<SActImagePoolWidget> MakeLaneWidgetForTrack(const TSharedRef<IActImageTrackBase>& InActImageTrack);

protected:
	TPanelChildren<SActImagePoolWidget::Slot> Children;// ** The track area's children.
};
