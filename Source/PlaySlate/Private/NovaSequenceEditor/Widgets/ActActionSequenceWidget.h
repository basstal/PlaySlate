#pragma once

#include "Utils/ActActionSequenceUtil.h"

#include "Widgets/SCompoundWidget.h"

class FActActionSequenceController;
class FActActionTimeSliderController;
class SActActionSequenceTreeView;
class SActActionSequenceTimeSliderWidget;
class SActActionSequenceTrackArea;

DECLARE_DELEGATE_TwoParams(FActActionOnGetAddMenuContent, FMenuBuilder&, TSharedRef<FActActionSequenceController>);

/**
 * Sequence的主要UI
 */
class SActActionSequenceWidget : public SCompoundWidget
{
	friend class FActActionSequenceController;
public:
	SLATE_BEGIN_ARGS(SActActionSequenceWidget)
		{
		}

		/** The current view range (seconds) */
		SLATE_ATTRIBUTE(ActActionSequence::FActActionAnimatedRange, ViewRange)

		/** The current clamp range (seconds) */
		SLATE_ATTRIBUTE(ActActionSequence::FActActionAnimatedRange, ClampRange)

		/**
		 * 动画播放的帧区间
		 * The playback range
		 */
		SLATE_ATTRIBUTE(TRange<FFrameNumber>, PlaybackRange)

		/** The selection range */
		SLATE_ATTRIBUTE(TRange<FFrameNumber>, SelectionRange)

		/** The Vertical Frames */
		SLATE_ATTRIBUTE(TSet<FFrameNumber>, VerticalFrames)

		// /** The Marked Frames */
		// SLATE_ATTRIBUTE(TArray<FMovieSceneMarkedFrame>, MarkedFrames)
		//
		// /** The Global Marked Frames */
		// SLATE_ATTRIBUTE(TArray<FMovieSceneMarkedFrame>, GlobalMarkedFrames)

		/** The current sub sequence range */
		SLATE_ATTRIBUTE(TOptional<TRange<FFrameNumber>>, SubSequenceRange)

		/** The playback status */
		SLATE_ATTRIBUTE(ActActionSequence::EPlaybackType, PlaybackStatus)

		/** Called when the user changes the playback range */
		SLATE_EVENT(ActActionSequence::OnFrameRangeChangedDelegate, OnPlaybackRangeChanged)

		/** Called when the user has begun dragging the playback range */
		SLATE_EVENT(FSimpleDelegate, OnPlaybackRangeBeginDrag)

		/** Called when the user has finished dragging the playback range */
		SLATE_EVENT(FSimpleDelegate, OnPlaybackRangeEndDrag)

		/** Called when the user changes the selection range */
		SLATE_EVENT(ActActionSequence::OnFrameRangeChangedDelegate, OnSelectionRangeChanged)

		/** Called when the user has begun dragging the selection range */
		SLATE_EVENT(FSimpleDelegate, OnSelectionRangeBeginDrag)

		/** Called when the user has finished dragging the selection range */
		SLATE_EVENT(FSimpleDelegate, OnSelectionRangeEndDrag)

		/** Called when the user has begun dragging a mark */
		SLATE_EVENT(FSimpleDelegate, OnMarkBeginDrag)

		/** Called when the user has finished dragging a mark */
		SLATE_EVENT(FSimpleDelegate, OnMarkEndDrag)

		/** Whether the playback range is locked */
		SLATE_ATTRIBUTE(bool, IsPlaybackRangeLocked)

		/** Called when the user toggles the play back range lock */
		SLATE_EVENT(FSimpleDelegate, OnTogglePlaybackRangeLocked)

		/** The current scrub position in (seconds) */
		SLATE_ATTRIBUTE(FFrameTime, ScrubPosition)

		/** The current scrub position text */
		SLATE_ATTRIBUTE(FString, ScrubPositionText)

		// /** The parent sequence that the scrub position display text is relative to */
		// SLATE_ATTRIBUTE( FMovieSceneSequenceID, ScrubPositionParent )

		// /** Called when the scrub position parent sequence is changed */
		// SLATE_EVENT( FOnScrubPositionParentChanged, OnScrubPositionParentChanged )

		// /** Attribute for the parent sequence chain of the current sequence */
		// SLATE_ATTRIBUTE( TArray<FMovieSceneSequenceID>, ScrubPositionParentChain )

		/** Called when the user changes the view range */
		SLATE_EVENT(ActActionSequence::OnViewRangeChangedDelegate, OnViewRangeChanged)

		/** Called when the user sets a marked frame */
		SLATE_EVENT(ActActionSequence::OnSetMarkedFrameDelegate, OnSetMarkedFrame)

		/** Called when the user adds a marked frame */
		SLATE_EVENT(ActActionSequence::OnAddMarkedFrameDelegate, OnAddMarkedFrame)

		/** Called when the user deletes a marked frame */
		SLATE_EVENT(ActActionSequence::OnDeleteMarkedFrameDelegate, OnDeleteMarkedFrame)

		/** Called when all marked frames should be deleted */
		SLATE_EVENT(FSimpleDelegate, OnDeleteAllMarkedFrames)

		/** Called when the user changes the clamp range */
		SLATE_EVENT(ActActionSequence::OnTimeRangeChangedDelegate, OnClampRangeChanged)

		/** Called to get the nearest key */
		SLATE_EVENT(ActActionSequence::OnGetNearestKeyDelegate, OnGetNearestKey)

		/** Called when the user has begun scrubbing */
		SLATE_EVENT(FSimpleDelegate, OnBeginScrubbing)

		/** Called when the user has finished scrubbing */
		SLATE_EVENT(FSimpleDelegate, OnEndScrubbing)

		/** Called when the user changes the scrub position */
		SLATE_EVENT(ActActionSequence::OnScrubPositionChangedDelegate, OnScrubPositionChanged)

		/** Called to populate the add combo button in the toolbar. */
		SLATE_EVENT(ActActionSequence::OnGetAddMenuContentDelegate, OnGetAddMenuContent)

		// /** Called when object is clicked. */
		// SLATE_EVENT(FOnBuildCustomContextMenuForGuid, OnBuildCustomContextMenuForGuid)

		/** Called when any widget contained within sequencer has received focus */
		SLATE_EVENT(FSimpleDelegate, OnReceivedFocus)

		// /** Called when something is dragged over the sequencer. */
		// SLATE_EVENT( FOptionalOnDragDrop, OnReceivedDragOver )
		//
		// /** Called when something is dropped onto the sequencer. */
		// SLATE_EVENT( FOptionalOnDragDrop, OnReceivedDrop )
		//
		// /** Called when an asset is dropped on the sequencer. Not called if OnReceivedDrop is bound and returned true. */
		// SLATE_EVENT( FOnAssetsDrop, OnAssetsDrop )
		//
		// /** Called when a class is dropped on the sequencer. Not called if OnReceivedDrop is bound and returned true. */
		// SLATE_EVENT( FOnClassesDrop, OnClassesDrop )
		//
		// /** Called when an actor is dropped on the sequencer. Not called if OnReceivedDrop is bound and returned true. */
		// SLATE_EVENT( FOnActorsDrop, OnActorsDrop )
		//
		// /** Called when a folder is dropped onto the sequencer. Not called if OnReceivedDrop is bound and returned true. */
		// SLATE_EVENT(FOnFoldersDrop, OnFoldersDrop)

		/** Extender to use for the add menu. */
		SLATE_ARGUMENT(TSharedPtr<FExtender>, AddMenuExtender)

		/** Extender to use for the toolbar. */
		SLATE_ARGUMENT(TSharedPtr<FExtender>, ToolbarExtender)
	SLATE_END_ARGS()

	SActActionSequenceWidget();
	virtual ~SActActionSequenceWidget() override;

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceController>& InSequenceController);

	//~Begin SCompoundWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SCompoundWidget interface

	ActActionSequence::FActActionPaintPlaybackRangeArgs GetSectionPlaybackRangeArgs() const;

protected:
	// TSharedPtr<SWidget> TickResolutionOverlay;
	/**
	 * 搜素框内文本改变的回调
	 */
	void OnOutlinerSearchChanged(const FText& Filter);
	/** Get the maximum height the pinned track area should be allowed to be */
	float GetPinnedAreaMaxHeight() const;
	/** Gets whether or not the pinned track area should be visible. */
	EVisibility GetPinnedAreaVisibility() const;
	/**
	 * 搜素和Track过滤功能
	 */
	TSharedPtr<SSearchBox> SearchBox;
	/**
	 * 当前View的Controller,转发所有控制Model的行为
	 */
	TWeakPtr<FActActionSequenceController> SequenceController;
	/**
	 * 用做左侧显示outliner和track区域的sequence tree view 
	 */
	TSharedPtr<SActActionSequenceTreeView> TreeView;
	/** The top time slider widget */
	TSharedPtr<SActActionSequenceTimeSliderWidget> TopTimeSlider;

	/** Time slider controller for this sequencer */
	TSharedPtr<FActActionTimeSliderController> TimeSliderController;
public:
	TSharedPtr<SActActionSequenceTreeView> GetTreeView() const
	{
		return TreeView;
	}

	TSharedPtr<INumericTypeInterface<double>> GetNumericType() const
	{
		return NumericTypeInterface;
	}

protected:
	/** Section area widget */
	TSharedPtr<SActActionSequenceTrackArea> TrackArea;

	/** Main Sequencer Area*/
	TSharedPtr<SVerticalBox> MainSequenceArea;
	/** The sequencer tree view for pinned tracks */
	TSharedPtr<SActActionSequenceTreeView> PinnedTreeView;
	/** Section area widget for pinned tracks*/
	TSharedPtr<SActActionSequenceTrackArea> PinnedTrackArea;

	/**
	 * FIX:搜索栏添加按钮？
	 */
	TSharedRef<SWidget> MakeAddButton();

	/**
	 * 添加菜单的内容
	 */
	TArray<TSharedPtr<FExtender>> AddMenuExtenders;
	/**
	 * 构造添加菜单的按钮SWidget
	 */
	TSharedRef<SWidget> MakeAddMenu();
	/**
	 * 添加菜单填充内容的事件
	 */
	FActActionOnGetAddMenuContent OnGetAddMenuContent;
	/**
	 * 往添加菜单中填充其内容
	 */
	void PopulateAddMenuContext(FMenuBuilder& MenuBuilder);


	/** 整个Sequence轨道的左侧和右侧分别占比 */
	float ColumnFillCoefficients[2];
	/**
	 * @return 0 - 左侧占比 1 - 右侧占比
	 */
	float GetColumnFillCoefficient(int32 ColumnIndex) const
	{
		return ColumnFillCoefficients[ColumnIndex];
	}

	/** Numeric type interface used for converting parsing and generating strings from numbers */
	TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
};
