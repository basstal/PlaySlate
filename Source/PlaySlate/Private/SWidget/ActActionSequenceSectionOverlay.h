#pragma once
#include "Editor/ActActionTimeSliderController.h"
#include "Utils/ActActionSequenceUtil.h"

class SActActionSequenceSectionOverlay : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceSectionOverlay)
		{
		}

		SLATE_ATTRIBUTE(bool, DisplayTickLines)
		SLATE_ATTRIBUTE(bool, DisplayScrubPosition)
		SLATE_ATTRIBUTE(bool, DisplayMarkedFrames)
		SLATE_ATTRIBUTE(ActActionSequence::FActActionPaintPlaybackRangeArgs, PaintPlaybackRangeArgs)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionTimeSliderController>& InTimeSliderController);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
protected:
	TSharedPtr<FActActionTimeSliderController> TimeSliderController;
	TAttribute<ActActionSequence::FActActionPaintPlaybackRangeArgs> PaintPlaybackRangeArgs;
	TAttribute<bool> bDisplayMarkedFrames;
	TAttribute<bool> bDisplayTickLines;
	TAttribute<bool> bDisplayScrubPosition;
};
