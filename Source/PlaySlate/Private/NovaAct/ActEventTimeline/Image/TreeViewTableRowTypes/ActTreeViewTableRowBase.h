#pragma once

class SActImageTreeViewTableRow;

class IActTreeViewTableRowBase
{
public:
	IActTreeViewTableRowBase();
	virtual ~IActTreeViewTableRowBase() = default;
	/**
	 * Generate a widget for the outliner for this track
	 *
	 * @param InTableRow 传入的 TableRow
	 * @return 生成的承载具体内容的 Widget
	 */
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) = 0;
};
