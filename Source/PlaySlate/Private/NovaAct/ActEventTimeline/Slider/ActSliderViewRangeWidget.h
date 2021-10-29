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
	double ViewBeginTime() const;
	/** 属性绑定 */
	double ViewEndTime() const;
	/**
	 * 属性值提交的回调
	 *
	 * @param InFrameValue
	 * @param InTextCommit
	 */
	void OnViewBeginTimeCommitted(double InFrameValue, ETextCommit::Type InTextCommit) const;
	/** 属性值提交的回调 */
	void OnViewEndTimeCommitted(double InFrameValue, ETextCommit::Type InTextCommit) const;
	/** 获得Spinbox Delta */
	double GetSpinboxDelta() const;
protected:
	TWeakPtr<SActSliderWidget> TimeSliderController;
};
