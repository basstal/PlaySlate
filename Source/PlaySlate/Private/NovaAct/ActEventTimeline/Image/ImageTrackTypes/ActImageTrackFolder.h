#pragma once
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackBase.h"

class FActImageTrackFolder : public IActImageTrackBase
{
public:
	FActImageTrackFolder();

	//~Begin IActImageTrackBase interface
	virtual ~FActImageTrackFolder() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForLaneWidget(const TSharedRef<SActImagePoolWidget>& InLaneWidget) override;
	//~End IActImageTrackBase interface
	
};
