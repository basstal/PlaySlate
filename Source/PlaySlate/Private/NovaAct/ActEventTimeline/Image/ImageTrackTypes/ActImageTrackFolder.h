#pragma once
#include "NovaAct/ActEventTimeline/Image/ImageTrackTypes/ActImageTrackBase.h"

class FActImageTrackFolder : public IActImageTrackBase
{
public:
	FActImageTrackFolder();

	//~Begin IActImageTrackBase interface
	virtual ~FActImageTrackFolder() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	//~End IActImageTrackBase interface

public:
	/** TreeView 与 Lane 共享参数 */
	TSharedPtr<FActImageTrackArgs> ActImageTrackArgs;
};
