#pragma once
#include "Common/NovaStruct.h"
#include "NovaAct/ActEventTimeline/Image/ActImagePoolWidget.h"

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
	/**
	 * 根据传入的 Lane Widget 生成对应的包装 Widget
	 *
	 * @param InLaneWidget
	 * @return 生成的承载具体内容的 Widget
	 */
	virtual TSharedRef<SWidget> GenerateContentWidgetForLaneWidget(const TSharedRef<SActImagePoolWidget>& InLaneWidget) = 0;

	/** TreeView 与 Lane 共享参数 */
	TSharedPtr<FActImageTrackArgs> ActImageTrackArgs;
	
};
