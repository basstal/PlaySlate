#pragma once

namespace NovaEnum
{
	/** Enumeration specifying whether we're playing forwards or backwards */
	enum class ENovaPlayDirection : uint8
	{
		Forwards,
		Backwards
	};

	enum class ENovaTreeViewTableRowType : uint8
	{
		// 根节点，不作任何显示
		None,
		// 包含某个类型的可伸缩节点，仅做显示用
		Folder,
		Notify
	};

	/** If we are dragging a scrubber or dragging to set the time range */
	enum class ENovaDragType : uint8
	{
		DRAG_SCRUBBING_TIME,
		// 拖拽选中 并 缩放至这段选中的时间
		DRAG_SETTING_RANGE,
		DRAG_PLAYBACK_START,
		DRAG_PLAYBACK_END,
		DRAG_SELECTION_START,
		DRAG_SELECTION_END,
		DRAG_MARK,
		DRAG_NONE
	};

	/** Enum representing supported scrubber styles */
	enum class EActSliderScrubberStyle : uint8
	{
		/** Scrubber is represented as a single thin line for the current time, with a constant-sized thumb. */
		Vanilla,

		/** Scrubber thumb occupies a full 'display rate' frame, with a single thin line for the current time. Tailored to frame-accuracy scenarios. */
		FrameBlock,
	};

	/** Enum specifying how to interpolate to a new view range */
	enum class ENovaViewRangeInterpolation : uint8
	{
		/** Use an externally defined animated interpolation */
		Animated,
		/** Set the view range immediately */
		Immediate,
	};

	enum class ENovaPlaybackType : uint8
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

	enum class ENovaNotifyStateHandleHit : uint8
	{
		Start,
		End,
		None
	};

	enum class ENovaTransportControls : uint8
	{
		ForwardStep,
		BackwardStep,
		ForwardEnd,
		BackwardEnd,
		ForwardPlay,
		BackwardPlay,
		ToggleLooping,
		Pause
	};
}
