#pragma once

#include "TimeSlider/ActActionTimeSliderWidget.h"
#include "Utils/ActActionSequenceUtil.h"

#include "Widgets/SCompoundWidget.h"

class FActActionSequenceController;
class FActActionTimeSliderController;
class SActActionSequenceTreeView;
class SActActionTimeSliderWidget;
class SActActionSequenceTrackArea;

/**
 * Sequence Tab的主要UI
 * 对应的Controller为FActActionSequenceController
 * 其下有子UI：
 *		SActActionTimeSliderWidget
 *		...
 */
class SActActionSequenceWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceWidget)
		{
		}

	SLATE_END_ARGS()

	virtual ~SActActionSequenceWidget() override;

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController);

	/**
	 * 搜素框内文本改变的回调
	 * @param Filter 回调的文本框内内容
	 */
	void OnOutlinerSearchChanged(const FText& Filter);
	/** Get the maximum height the pinned track area should be allowed to be */
	float GetPinnedAreaMaxHeight() const;
	/**
	 * @return 构造AddTrack菜单Widget 
	 */
	TSharedRef<SWidget> MakeAddMenu();
	/**
	 * @return 获得TimeSlider的Controller
	 */
	TSharedRef<FActActionTimeSliderController> GetActActionTimeSliderController() const;
protected:
	/**
	 * 当前View的Controller，转发所有逻辑行为
	 * Widget中所有Controller应该都是WeakPtr
	 */
	TWeakPtr<FActActionSequenceController> ActActionSequenceController;

	/** The top time slider widget */
	TSharedPtr<SActActionTimeSliderWidget> SequenceTimeSliderWidget;
	/** Main Sequencer Area*/
	TSharedPtr<SVerticalBox> MainSequenceArea;
	/** 整个Sequence轨道的左侧和右侧分别占比 */
	float ColumnFillCoefficients[2];

public:
	TSharedRef<FActActionSequenceController> GetActActionSequenceController() const
	{
		return ActActionSequenceController.Pin().ToSharedRef();
	}
	/**
	 * 获得一个存储好的百分比占比值
	 *
	 * @param ColumnIndex 0 - 左侧占比 1 - 右侧占比
	 * @return 占比值[0,1]
	 */
	float GetColumnFillCoefficient(int32 ColumnIndex) const
	{
		return ColumnFillCoefficients[ColumnIndex];
	}
};
