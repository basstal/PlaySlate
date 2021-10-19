#pragma once

namespace ActActionSequence
{
	enum class ESequenceNodeType : uint8
	{
		// 根节点，不作任何显示
		Root,
		// 包含某个类型的可伸缩节点，仅做显示用
		Folder,
		// 包含开始时间和结束时间的节点
		State,
		// 仅包含开始时间的节点
		Notify,
	};

	/** If we are dragging a scrubber or dragging to set the time range */
	enum class EDragType : uint8
	{
		DRAG_SCRUBBING_TIME,
		DRAG_SETTING_RANGE,
		DRAG_PLAYBACK_START,
		DRAG_PLAYBACK_END,
		DRAG_SELECTION_START,
		DRAG_SELECTION_END,
		DRAG_MARK,
		DRAG_NONE
	};

	/** Enum representing supported scrubber styles */
	enum class ESequencerScrubberStyle : uint8
	{
		/** Scrubber is represented as a single thin line for the current time, with a constant-sized thumb. */
		Vanilla,

		/** Scrubber thumb occupies a full 'display rate' frame, with a single thin line for the current time. Tailored to frame-accuracy scenarios. */
		FrameBlock,
	};

	/** Enum specifying how to interpolate to a new view range */
	enum class EActActionViewRangeInterpolation : uint8
	{
		/** Use an externally defined animated interpolation */
		Animated,
		/** Set the view range immediately */
		Immediate,
	};

	enum class EPlaybackType : uint8
	{
		Stopped,
		Playing,
		Recording,
		Scrubbing,
		Jumping,
		Stepping,
		Paused,
		MAX
	};

	enum class ESectionOverlayWidgetType : uint8
	{
		TickLines,
		ScrubPosition
	};
}
