#pragma once

#include "Common/NovaStruct.h"

class SActActionSequenceSectionOverlayWidget;
class FActEventTimelineSlider;

class FActEventTimelineImage : public TSharedFromThis<FActEventTimelineImage>
{
public:
	FActEventTimelineImage(const TSharedRef<FActEventTimelineSlider>& InActActionTimeSliderController);

	~FActEventTimelineImage();

	/**
	 * 构造对应的Widget
	 *
	 * @param InOverlayWidgetType 构造Widget的类型，决定了Widget中一些开关选项
	 */
	void MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType InOverlayWidgetType);

	/**
	 * 绘制显示帧
	 *
	 * @param OutDrawElements
	 * @param ViewRange
	 * @param RangeToScreen
	 * @param InArgs
	 */
	void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionDrawTickArgs& InArgs) const;

	/**
	 * @return 获得TimeSlider相关参数
	 */
	ActActionSequence::FActActionTimeSliderArgs& GetTimeSliderArgs() const;

protected:
	/**
	 * 父Controller的弱引用
	 */
	TWeakPtr<FActEventTimelineSlider> ActActionTimeSliderController;

	/**
	 * 对应的Widget
	 */
	TSharedPtr<SActActionSequenceSectionOverlayWidget> ActActionSequenceSectionOverlayWidget;

	/**
	 * 相关参数
	 */
public:
	TSharedRef<SActActionSequenceSectionOverlayWidget> GetActActionSequenceSectionOverlayWidget() const
	{
		return ActActionSequenceSectionOverlayWidget.ToSharedRef();
	}
};
