#pragma once
#include "NovaAct/ActEventTimeline/Image/TreeViewTableRowTypes/ActTreeViewTableRowBase.h"

class FActTreeViewTableRowFolder : public IActTreeViewTableRowBase
{
public:
	FActTreeViewTableRowFolder();

	//~Begin IActTreeViewTableRowBase interface
	virtual ~FActTreeViewTableRowFolder() override;
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) override;
	//~End IActTreeViewTableRowBase interface

public:
	/** Display name to use */
	FText DisplayName;
	/** Tooltip text to use */
	FText ToolTipText;
	/** Whether this is a header track */
	bool bIsHeaderTableRow : 1;
};
