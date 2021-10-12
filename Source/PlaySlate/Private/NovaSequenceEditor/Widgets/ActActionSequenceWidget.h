#pragma once

#include "TimeSlider/ActActionSequenceTimeSliderWidget.h"
#include "Utils/ActActionSequenceUtil.h"

#include "Widgets/SCompoundWidget.h"

class FActActionSequenceController;
class FActActionTimeSliderController;
class SActActionSequenceTreeView;
class SActActionSequenceTimeSliderWidget;
class SActActionSequenceTrackArea;

/**
 * Sequence Tab的主要UI
 * 对应的Controller为FActActionSequenceController
 * 其下有子UI：
 *		SActActionSequenceTimeSliderWidget
 *		SActActionSequenceTrackArea
 *		SActActionSequenceTreeView
 *		...
 */
class SActActionSequenceWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SActActionSequenceWidget)
		{
		}

		/**
		 * 动画播放的帧区间
		 */
		SLATE_ATTRIBUTE(TRange<FFrameNumber>, PlaybackRange)
		/** The playback status */
		SLATE_ATTRIBUTE(ActActionSequence::EPlaybackType, PlaybackStatus)
		/** The selection range */
		SLATE_ATTRIBUTE(TRange<FFrameNumber>, SelectionRange)
		/** Called when the user changes the playback range */
		SLATE_EVENT(ActActionSequence::OnFrameRangeChangedDelegate, OnPlaybackRangeChanged)
		/** The current scrub position in (seconds) */
		SLATE_ATTRIBUTE(FFrameTime, ScrubPosition)
		/** The current scrub position text */
		SLATE_ATTRIBUTE(FString, ScrubPositionText)
		/** Called when the user has begun scrubbing */
		SLATE_EVENT(FSimpleDelegate, OnBeginScrubbing)
		/** Called when the user has finished scrubbing */
		SLATE_EVENT(FSimpleDelegate, OnEndScrubbing)
		/** Called when the user changes the scrub position */
		SLATE_EVENT(ActActionSequence::OnScrubPositionChangedDelegate, OnScrubPositionChanged)
		/** Called to populate the add combo button in the toolbar. */
		SLATE_EVENT(ActActionSequence::OnGetAddMenuContentDelegate, OnGetAddMenuContent)
		/** Called when any widget contained within sequencer has received focus */
		SLATE_EVENT(FSimpleDelegate, OnReceivedFocus)
		/** Extender to use for the add menu. */
		SLATE_ARGUMENT(TSharedPtr<FExtender>, AddMenuExtender)
		/** Extender to use for the toolbar. */
		SLATE_ARGUMENT(TSharedPtr<FExtender>, ToolbarExtender)
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
	 * 往AddTrack菜单中填充内容
	 * 
	 * @param MenuBuilder 被填充的菜单对象
	 */
	void PopulateAddMenuContext(FMenuBuilder& MenuBuilder);
protected:
	/**
	 * 当前View的Controller，转发所有逻辑行为
	 * Widget中所有Controller应该都是WeakPtr
	 */
	TWeakPtr<FActActionSequenceController> SequenceController;
	/**
	 * 用做左侧显示Outliner和Track区域的Sequence的TreeView 
	 */
	TSharedPtr<SActActionSequenceTreeView> TreeView;
	/** The top time slider widget */
	TSharedPtr<SActActionSequenceTimeSliderWidget> SequenceTimeSliderWidget;
	/** Section area widget */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;
	/** Main Sequencer Area*/
	TSharedPtr<SVerticalBox> MainSequenceArea;
	/** 整个Sequence轨道的左侧和右侧分别占比 */
	float ColumnFillCoefficients[2];
	
public:
	TSharedRef<FActActionTimeSliderController> GetTimeSliderController() const
	{
		return SequenceTimeSliderWidget->GetTimeSliderController();
	}

	TSharedRef<SActActionSequenceTreeView> GetTreeView() const
	{
		return TreeView.ToSharedRef();
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
