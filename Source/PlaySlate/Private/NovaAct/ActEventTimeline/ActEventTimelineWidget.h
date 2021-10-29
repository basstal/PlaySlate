#pragma once

#include "Common/NovaDataBinding.h"
#include "Common/NovaDelegate.h"


class SActSliderWidget;
class SActImageTreeView;
class SActActionTimeSliderWidget;
class SActImageTrackAreaPanel;

using namespace NovaDelegate;
/**
 * Tab的主要UI
 */
class SActEventTimelineWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActEventTimelineWidget) { }
	SLATE_END_ARGS()

	SActEventTimelineWidget();
	virtual ~SActEventTimelineWidget() override;

	void Construct(const FArguments& InArgs);

	/**
	 * 搜素框内文本改变的回调
	 * @param Filter 回调的文本框内内容
	 */
	void OnOutlinerSearchChanged(const FText& Filter);
	/**
	 * 构造AddTrack菜单Widget
	 * @return 菜单Widget
	 */
	TSharedRef<SWidget> BuildAddTrackMenuWidget();
protected:
	TSharedPtr<SActSliderWidget> ActEventTimelineSliderWidget;
	/** 过滤用的关键字 */
	FText InFilter;
	/** 所有已注册的CreateTrackEditor代理方法，在FActActionTrackEditorBase的子类中实现 */
	TArray<OnCreateTrackEditorDelegate> TrackEditorDelegates;
	/** List of tools we own */
	TArray<TSharedPtr<FActActionTrackEditorBase>> TrackEditors;
};
