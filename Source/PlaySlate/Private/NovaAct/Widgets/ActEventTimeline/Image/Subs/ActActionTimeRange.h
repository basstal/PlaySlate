#pragma once

class FActEventTimelineSlider;
class SActActionTimeRangeSlider;

class SActActionTimeRange : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionTimeRange)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineSlider>& InTimeSliderController, const TSharedRef<SActActionTimeRangeSlider>& InTimeRangeSlider);

	/** 属性绑定 */
	double ViewStartTime() const;
	/** 属性绑定 */
	double ViewEndTime() const;
	/** 属性值提交的回调 */
	void OnViewStartTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const;
	/** 属性值提交的回调 */
	void OnViewEndTimeCommitted(double NewValue, ETextCommit::Type InTextCommit) const;
	/** 属性值改变的回调 */
	void OnViewTimeChanged(double NewValue, bool bIsEndValue = false) const;
	/** 获得Spinbox Delta */
	double GetSpinboxDelta() const;
protected:
	TWeakPtr<FActEventTimelineSlider> TimeSliderController;
};
