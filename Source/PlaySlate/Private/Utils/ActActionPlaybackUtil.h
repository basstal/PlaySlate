#pragma once

namespace ActActionSequence
{
	/** Enumeration specifying whether we're playing forwards or backwards */
	enum class EPlayDirection : uint8
	{
		Forwards,
		Backwards
	};

	/** ActActionSequence evaluation context. Should remain bitwise copyable, and contain no external state since this has the potential to be used on a thread */
	struct FActActionEvaluationRange
	{
		/**
		 * Construct this range from a single fixed time
		 */
		FActActionEvaluationRange(FFrameTime InTime, FFrameRate InFrameRate)
			: EvaluationRange(InTime),
			  CurrentFrameRate(InFrameRate),
			  Direction(EPlayDirection::Forwards),
			  TimeOverride(FFrameNumber(TNumericLimits<int32>::Lowest()))
		{
		}

		/**
		 * Construct this range from a raw range and a direction
		 */
		FActActionEvaluationRange(TRange<FFrameTime> InRange, FFrameRate InFrameRate, EPlayDirection InDirection)
			: EvaluationRange(InRange),
			  CurrentFrameRate(InFrameRate),
			  Direction(InDirection),
			  TimeOverride(FFrameNumber(TNumericLimits<int32>::Lowest()))
		{
		}

		/**
		 * Construct this range from 2 times, and whether the range should include the previous time or not
		 */
		FActActionEvaluationRange(FFrameTime InCurrentTime, FFrameTime InPreviousTime, FFrameRate InFrameRate, bool bInclusivePreviousTime = false)
			: EvaluationRange(FActActionEvaluationRange::CalculateEvaluationRange(InCurrentTime, InPreviousTime, bInclusivePreviousTime)),
			  CurrentFrameRate(InFrameRate),
			  Direction((InCurrentTime - InPreviousTime >= FFrameTime()) ? EPlayDirection::Forwards : EPlayDirection::Backwards),
			  TimeOverride(TNumericLimits<int32>::Lowest())
		{
		}

		static TRange<FFrameTime> CalculateEvaluationRange(FFrameTime CurrentTime, FFrameTime PreviousTime, bool bInclusivePreviousTime);
		/**
		 * Convert a frame time range to a frame number range comprising all the frame numbers traversed in the range
		 */
		static TRange<FFrameNumber> TimeRangeToNumberRange(const TRange<FFrameTime>& InFrameTimeRange);

		/**
		 * Convert a frame number range to a frame time range
		 */
		static TRange<FFrameTime> NumberRangeToTimeRange(const TRange<FFrameNumber>& InFrameTimeRange);

		/**
		 * Get the range that we should be evaluating
		 */
		FORCEINLINE TRange<FFrameTime> GetRange() const
		{
			return EvaluationRange;
		}

		/**
		 * Get the range of frame numbers traversed over this evaluation range, not including partial frames
		 */
		FORCEINLINE TRange<FFrameNumber> GetFrameNumberRange() const
		{
			return TimeRangeToNumberRange(EvaluationRange);
		}

		/**
		 * Get the range of frame numbers traversed over this evaluation range by flooring the lower bound, and ceiling the upper bound.
		 * For example: a time range of [1.5, 5.6] will yield the equivalent of [1, 6). A time range of (2.0, 2.9) will yield the equivalent of [2,3).
		 */
		TRange<FFrameNumber> GetTraversedFrameNumberRange() const;

		/**
		 * Get the direction to evaluate our range
		 */
		FORCEINLINE EPlayDirection GetDirection() const
		{
			return Direction;
		}

		/**
		 * Get the current time of evaluation.
		 */
		FORCEINLINE FFrameTime GetTime() const
		{
			if (TimeOverride != TNumericLimits<int32>::Lowest())
			{
				return TimeOverride;
			}

			return Direction == EPlayDirection::Forwards ? EvaluationRange.GetUpperBoundValue() : EvaluationRange.GetLowerBoundValue();
		}

		/**
		 * Get the absolute amount of time that has passed since the last update (will always be >= 0)
		 */
		FORCEINLINE FFrameTime GetDelta() const
		{
			return EvaluationRange.Size<FFrameTime>();
		}

		/**
		 * Get the previous time of evaluation. Should not generally be used. Prefer GetRange instead.
		 */
		FORCEINLINE FFrameTime GetPreviousTime() const
		{
			return Direction == EPlayDirection::Forwards ? EvaluationRange.GetLowerBoundValue() : EvaluationRange.GetUpperBoundValue();
		}

		/**
		 * Get the current time offset by the specified amount in the direction of play
		 */
		FORCEINLINE FFrameTime GetOffsetTime(FFrameTime InOffset) const
		{
			FFrameTime Now = GetTime();
			return Direction == EPlayDirection::Forwards ? Now + InOffset : Now - InOffset;
		}

		/**
		 * Override the time that we're actually evaluating at
		 */
		FORCEINLINE void OverrideTime(FFrameNumber InTimeOverride)
		{
			TimeOverride = InTimeOverride;
		}

		/**
		 * Get the framerate that this context's times are in
		 * @return The framerate that all times are relative to
		 */
		FORCEINLINE FFrameRate GetFrameRate() const
		{
			return CurrentFrameRate;
		}

	protected:
		/** The range to evaluate */
		TRange<FFrameTime> EvaluationRange;

		/** The framerate of the current sequence. */
		FFrameRate CurrentFrameRate;

		/** Whether to evaluate the range forwards, or backwards */
		EPlayDirection Direction;

		/** Overridden current time (Does not manipulate the actual evaluated range) */
		FFrameNumber TimeOverride;
	};

	/** Helper class designed to abstract the complexity of calculating evaluation ranges for previous times and fixed time intervals */
	struct FActActionPlaybackPosition
	{
		FActActionPlaybackPosition()
			: InputRate(0, 0),
			  OutputRate(0, 0)
		{
		}

		/**
		 * @return The input frame rate that all frame times provided to this class will be interpreted as
		 */
		FORCEINLINE FFrameRate GetInputRate() const
		{
			return InputRate;
		}

		/**
		 * @return The output frame rate that all frame times returned from this class will be interpreted as
		 */
		FORCEINLINE FFrameRate GetOutputRate() const
		{
			return OutputRate;
		}

	public:
		/**
		 * Assign the input and output rates that frame times should be interpreted as.
		 *
		 * @param InInputRate           The framerate to interpret any frame time provided to this class
		 * @param InOutputRate          The framerate to use when returning any frame range from this class
		 */
		void SetTimeBase(FFrameRate InInputRate, FFrameRate InOutputRate);

		/**
		 * Reset this position to the specified time.
		 * @note Future calls to 'PlayTo' will include this time in its resulting evaluation range
		 */
		void Reset(FFrameTime StartPos);

		/**
		 * Get the last position that was set, in InputRate space
		 */
		FFrameTime GetCurrentPosition() const { return CurrentPosition; }

		/**
		 * Get the last actual time that was evaluated during playback, in InputRate space.
		 */
		TOptional<FFrameTime> GetLastPlayEvalPosition() const { return PreviousPlayEvalPosition; }

		/**
		 * Jump to the specified input time.
		 * @note Will reset previous play position. Any subsequent call to 'PlayTo' will include NewPosition.
		 *
		 * @param NewPosition         The new frame time to set, in InputRate space
		 * @return A range encompassing only the specified time, in OutputRate space.
		 */
		FActActionEvaluationRange JumpTo(FFrameTime NewPosition);

		/**
		 * Play from the previously evaluated play time, to the specified time
		 *
		 * @param NewPosition         The new frame time to set, in InputRate space
		 * @return An evaluation range from the previously evaluated time to the specified time, in OutputRate space.
		 */
		FActActionEvaluationRange PlayTo(FFrameTime NewPosition);

		/**
		 * Get a range that encompasses the last evaluated range in OutputRate space.
		 * @return An optional evaluation range in OutputRate space.
		 */
		TOptional<FActActionEvaluationRange> GetLastRange() const
		{
			return LastRange;
		};

		/**
		 * Get a range encompassing only the current time, if available (in OutputRate space)
		 * @return An optional evaluation range in OutputRate space.
		 */
		FActActionEvaluationRange GetCurrentPositionAsRange() const;


	protected:
		/**
		 * Check this class's invariants
		 */
		void CheckInvariants() const;

		/** The framerate to be used when interpreting frame time values provided to this class (i.e. display rate) */
		FFrameRate InputRate;

		/** The framerate to be used when returning frame time values from this class (i.e. tick resolution) */
		FFrameRate OutputRate;

		// /** The type of evaluation to use */
		// EMovieSceneEvaluationType EvaluationType;

		/** The current time position set, in 'InputRate' time-space. */
		FFrameTime CurrentPosition;

		/** The previously evaluated position when playing, in 'InputRate' time-space. */
		TOptional<FFrameTime> PreviousPlayEvalPosition;

		/** The previously evaluated range if available, in 'OutputRate' time-space */
		TOptional<FActActionEvaluationRange> LastRange;
	};

	struct FActActionAutoScrubTarget
	{
		FActActionAutoScrubTarget(FFrameTime InDestinationTime, FFrameTime InSourceTime, double InStartTime)
			: DestinationTime(InDestinationTime),
			  SourceTime(InSourceTime),
			  StartTime(InStartTime)
		{
		}

		FFrameTime DestinationTime;
		FFrameTime SourceTime;
		double StartTime;
	};

	struct FActActionTrackAreaArgs
	{
		TAttribute<float> ViewInputMin;
		TAttribute<float> ViewInputMax;
	};
}
