#pragma once

class FActEventTimelineSlider;

class SActActionTimeSliderWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionTimeSliderWidget) { }

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineSlider>& InTimeSliderController);

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	//~End SWidget interface
protected:
	/**
	 * Sequence Controller
	 */
	TWeakPtr<FActEventTimelineSlider> TimeSliderController;
};
