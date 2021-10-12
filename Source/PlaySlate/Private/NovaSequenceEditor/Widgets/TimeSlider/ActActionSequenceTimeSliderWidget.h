#pragma once

#include "NovaSequenceEditor/Controllers/TimeSlider/ActActionTimeSliderController.h"
#include "Utils/ActActionSequenceUtil.h"

class FActActionTimeSliderController;

class SActActionSequenceTimeSliderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceTimeSliderWidget)
			: _MirrorLabels(false)
		{
		}

		/* If we should mirror the labels on the timeline */
		SLATE_ARGUMENT(bool, MirrorLabels)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionTimeSliderController>& InTimeSliderController);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~End SWidget interface

	void DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, ActActionSequence::FActActionDrawTickArgs& InArgs) const;
	int32 DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	int32 DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
	int32 DrawSelectionRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const;
protected:
	/**
	 * Sequence Controller
	 */
	TWeakPtr<FActActionTimeSliderController> TimeSliderController;
	/**
	 * 
	 */
	bool bMirrorLabels;

	ActActionSequence::EDragType MouseDragType;

	/** TRANS_EN:Geometry on mouse down */
	FGeometry MouseDownGeometry;
	/** TRANS_EN:Mouse down position range */
	FVector2D MouseDownPosition[2];
public:
	TSharedRef<FActActionTimeSliderController> GetTimeSliderController() const
	{
		return TimeSliderController.Pin().ToSharedRef();
	}

	ActActionSequence::FActActionTimeSliderArgs GetTimeSliderArgs() const
	{
		return GetTimeSliderController()->GetTimeSliderArgs();
	}
};
