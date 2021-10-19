﻿#pragma once

#include "Utils/ActActionSequenceUtil.h"

class SActActionSequenceSectionOverlayWidget;
class FActActionTimeSliderController;

class FActActionSequenceSectionOverlayController : public TSharedFromThis<FActActionSequenceSectionOverlayController>
{
public:
	FActActionSequenceSectionOverlayController(const TSharedRef<FActActionTimeSliderController>& InActActionTimeSliderController);
	~FActActionSequenceSectionOverlayController();

	/**
	 * 构造对应的Widget
	 *
	 * @param InOverlayWidgetType 构造Widget的类型，决定了Widget中一些开关选项
	 */
	void MakeSequenceSectionOverlayWidget(ActActionSequence::ESectionOverlayWidgetType InOverlayWidgetType);
	/**
	 * TODO:可能不需要的方法
	 */
	int32 DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	/**
	 * TODO:可能不需要的方法
	 */
	int32 DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	/**
	 * TODO:
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
	TWeakPtr<FActActionTimeSliderController> ActActionTimeSliderController;
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
