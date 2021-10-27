#pragma once

#include "Common/NovaStruct.h"

class SActImageScrubPosition;
class SActSliderWidget;

using namespace NovaDelegate;
using namespace NovaStruct;

class SActImageThickLine : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActImageThickLine) { }

		// SLATE_ATTRIBUTE(bool, DisplayTickLines)
		// SLATE_ATTRIBUTE(bool, DisplayScrubPosition)
		SLATE_ATTRIBUTE(bool, DisplayMarkedFrames)
		SLATE_ATTRIBUTE(FActActionPaintPlaybackRangeArgs, PaintPlaybackRangeArgs)

	SLATE_END_ARGS()

	virtual ~SActImageThickLine() override;

	void Construct(const FArguments& InArgs);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
	// /**
	//  * 构造对应的Widget
	//  *
	//  * @param InOverlayWidgetType 构造Widget的类型，决定了Widget中一些开关选项
	//  */
	// void MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType InOverlayWidgetType);
	//
	// /**
	//  * 绘制显示帧
	//  *
	//  * @param OutDrawElements
	//  * @param ViewRange
	//  * @param RangeToScreen
	//  * @param InArgs
	//  */
	// void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActActionScrubRangeToScreen& RangeToScreen, const FActActionDrawTickArgs& InArgs) const;

	// /**
	//  * @return 获得TimeSlider相关参数
	//  */
	// FActEventTimelineArgs& GetTimeSliderArgs() const;

protected:
	/**
	 * 父Controller的弱引用
	 */
	TWeakPtr<SActSliderWidget> ActActionTimeSliderController;

	/**
	 * 对应的Widget
	 */
	TSharedPtr<SActImageScrubPosition> ActActionSequenceSectionOverlayWidget;

	/**
	 * 相关参数
	 */
	/**
	 * TODO:
	 */
	TAttribute<FActActionPaintPlaybackRangeArgs> PaintPlaybackRangeArgs;
	/**
	 * TODO:
	 */
	TAttribute<bool> bDisplayMarkedFrames;
public:
	TSharedRef<SActImageScrubPosition> GetActActionSequenceSectionOverlayWidget() const
	{
		return ActActionSequenceSectionOverlayWidget.ToSharedRef();
	}
};
