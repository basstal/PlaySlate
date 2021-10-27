#pragma once

class SActSliderWidget;
class SActSliderViewRangeBarWidget;

class SActSliderViewRangeWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActSliderViewRangeWidget) { }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

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
	TWeakPtr<SActSliderWidget> TimeSliderController;
};
