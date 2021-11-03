#pragma once

#include "Common/NovaDelegate.h"
#include "Widgets/SCompoundWidget.h"


class SActSliderWidget;
class SActImageTreeView;
class SActActionTimeSliderWidget;
class SActImagePoolAreaPanel;

using namespace NovaDelegate;
/**
 * Tab的主要UI
 */
class SActEventTimelineWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActEventTimelineWidget) { }
	SLATE_END_ARGS()

	virtual ~SActEventTimelineWidget() override;

	void Construct(const FArguments& InArgs);

	/**
	 * 搜素框内文本改变的回调
	 * @param InFilter 回调的文本框内内容
	 */
	void OnTreeViewFilterChanged(const FText& InFilter);
protected:
	TSharedPtr<SActSliderWidget> ActEventTimelineSliderWidget;
};
