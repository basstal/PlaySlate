#pragma once

#include "NovaAct/ActEventTimeline/ActEventTimelineImage.h"
#include "Common/NovaStruct.h"

using namespace NovaDelegate;
using namespace NovaStruct;

class FActEventTimelineSlider;

class SActActionSequenceSectionOverlayWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceSectionOverlayWidget)
		{ }

		SLATE_ATTRIBUTE(bool, DisplayTickLines)
		SLATE_ATTRIBUTE(bool, DisplayScrubPosition)
		SLATE_ATTRIBUTE(bool, DisplayMarkedFrames)
		SLATE_ATTRIBUTE(FActActionPaintPlaybackRangeArgs, PaintPlaybackRangeArgs)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineImage>& InActActionSequenceSectionOverlayController);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
protected:
	/**
	 * 这个Widget的Controller
	 */
	TWeakPtr<FActEventTimelineImage> ActActionSequenceSectionOverlayController;
	/**
	 * TODO:
	 */
	TAttribute<FActActionPaintPlaybackRangeArgs> PaintPlaybackRangeArgs;
	/**
	 * TODO:
	 */
	TAttribute<bool> bDisplayMarkedFrames;
	/**
	 * TODO:
	 */
	TAttribute<bool> bDisplayTickLines;
	/**
	 * TODO:
	 */
	TAttribute<bool> bDisplayScrubPosition;
};
