#pragma once
#include "Common/NovaStruct.h"

class SActImageTreeViewTableRow;

using namespace NovaStruct;
class IActImageTrackBase : public TSharedFromThis<IActImageTrackBase>
{
public:
	virtual ~IActImageTrackBase() = default;
	/**
	 * 根据传入的 TreeView TableRow 生成对应的包装 TableRow 的 Widget
	 *
	 * @param InTableRow 传入的 TableRow
	 * @return 生成的承载具体内容的 Widget
	 */
	virtual TSharedRef<SWidget> GenerateContentWidgetForTableRow(const TSharedRef<SActImageTreeViewTableRow>& InTableRow) = 0;
};
