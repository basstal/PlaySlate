#pragma once

#include "Common/NovaDelegate.h"


namespace NovaStruct
{
	using namespace NovaEnum;
	using namespace NovaDelegate;

	/** EventTimeline 所有Widget可能需要共享的一些参数，使用数据绑定以避免参数透传 */
	struct FActEventTimelineArgs
	{
		FActEventTimelineArgs()
			: ViewRange(new TRange<float>(0.0f, 1.0f)),
			  ClampRange(TRange<float>(0.0f, 1.0f)),
			  TickResolution(60, 1),
			  CurrentTime(new FFrameTime(0)),
			  PlaybackStatus(ENovaPlaybackType::Stopped),
			  AllowZoom(true) {}

		TSharedPtr<TRange<float>> ViewRange;// 当前可见的区域，这个值可以比动画播放区间小，例如放大显示时
		TRange<float> ClampRange;           // 最大可调整的区域，与AnimSequence动画的播放时长相同，范围[0, PlayLength]

		FFrameRate TickResolution;         // 当前使用的帧率
		TSharedPtr<FFrameTime> CurrentTime;// 当前动画所在的时间节点
		ENovaPlaybackType PlaybackStatus;  // 当前的播放状态

		bool AllowZoom;                                                // If we are allowed to zoom
		TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;// Numeric Type interface for converting between frame numbers and display formats.
	};

	// struct FActEventTimelineEvents
	// {
	// 	OnScrubPositionChangedDelegate OnScrubPositionChanged;// Called when the scrub position changes 
	// 	FSimpleDelegate OnBeginScrubberMovement;              // Called right before the scrubber begins to move 
	// 	FSimpleDelegate OnEndScrubberMovement;                // Called right after the scrubber handle is released by the user 
	// };

	/** Utility struct for converting between scrub range space and local/absolute screen space */
	struct FActActionScrubRangeToScreen
	{
		double ViewStart;

		float PixelsPerInput;

		FActActionScrubRangeToScreen(const TRange<float>& InViewInput, const FVector2D& InWidgetSize)
		{
			const float ViewInputRange = InViewInput.Size<double>();

			ViewStart = InViewInput.GetLowerBoundValue();
			PixelsPerInput = ViewInputRange > 0 ? (InWidgetSize.X / ViewInputRange) : 0;
		}

		/** Local Widget Space -> Curve Input domain. */
		double LocalXToInput(float ScreenX) const
		{
			return PixelsPerInput > 0 ? (ScreenX / PixelsPerInput) + ViewStart : ViewStart;
		}

		/** Curve Input domain -> local Widget Space */
		float InputToLocalX(double Input) const
		{
			return (Input - ViewStart) * PixelsPerInput;
		}
	};


	struct FActActionTrackAreaArgs
	{
		int BeginTime; // ** 开始时间
		float Duration;// ** 时间长度

		// float GetBeginTime() const
		// {
		// 	return Begin.Get() * TickResolution.Get().AsInterval();
		// }
		//
		float GetEndTime() const
		{
			return BeginTime + Duration;
		}

		//
		// float GetDurationTime() const
		// {
		// 	return (End.Get() - Begin.Get()) * TickResolution.Get().AsInterval();
		// }
		//
		// float GetPlayLength() const
		// {
		// 	return ViewInputMax.Get() - ViewInputMin.Get();
		// }
	};


	struct FActActionDrawTickArgs
	{
		/** Geometry of the area */
		FGeometry AllottedGeometry;

		/** Culling rect of the area */
		FSlateRect CullingRect;

		/** Color of each tick */
		FLinearColor TickColor;

		/** Offset in Y where to start the tick */
		float TickOffset;

		/** Height in of major ticks */
		float MajorTickHeight;

		/** Start layer for elements */
		int32 StartLayer;

		/** Draw effects to apply */
		ESlateDrawEffect DrawEffects;

		/** Whether or not to only draw major ticks */
		bool bOnlyDrawMajorTicks;

		/** Whether or not to mirror labels */
		bool bMirrorLabels;
	};

	struct FActActionPaintPlaybackRangeArgs
	{
		/** Brush to use for the start bound */
		const FSlateBrush* StartBrush;

		/** Brush to use for the end bound */
		const FSlateBrush* EndBrush;

		/** The width of the above brushes, in slate units */
		float BrushWidth;

		/** level of opacity for the fill color between the range markers */
		float SolidFillOpacity;
	};

	struct FActActionScrubberMetrics
	{
		/** The extents of the current frame that the scrubber is on, in pixels */
		TRange<float> FrameExtentsPx;

		/** The pixel range that the scrubber handle (thumb) occupies */
		TRange<float> HandleRangePx;

		/** The style of the scrubber handle */
		ENovaSequencerScrubberStyle Style;

		/** The style of the scrubber handle */
		bool bDrawExtents;
	};

	struct FActActionAnimatedPropertyKey
	{
		/**
		 * The name of the type of property that can be animated (i.e. FBoolProperty)
		 */
		FName PropertyTypeName;

		/**
		 * The name of the type of object that can be animated inside the property (i.e. the name of the struct or object for FStructProperty or FObjectProperty). NAME_None for any properties.
		 */
		FName ObjectTypeName;

		friend uint32 GetTypeHash(FActActionAnimatedPropertyKey InKey)
		{
			return GetTypeHash(InKey.PropertyTypeName) ^ GetTypeHash(InKey.ObjectTypeName);
		}

		friend bool operator==(FActActionAnimatedPropertyKey A, FActActionAnimatedPropertyKey B)
		{
			return A.PropertyTypeName == B.PropertyTypeName && A.ObjectTypeName == B.ObjectTypeName;
		}

		static FActActionAnimatedPropertyKey FromProperty(const FProperty* Property)
		{
			FActActionAnimatedPropertyKey Definition;
			Definition.PropertyTypeName = Property->GetClass()->GetFName();

			if (const FStructProperty* StructProperty = CastField<const FStructProperty>(Property))
			{
				Definition.ObjectTypeName = StructProperty->Struct->GetFName();
			}
			else if (const FObjectPropertyBase* ObjectProperty = CastField<const FObjectPropertyBase>(Property))
			{
				if (ObjectProperty->PropertyClass)
				{
					Definition.ObjectTypeName = ObjectProperty->PropertyClass->GetFName();
				}
			}
			else if (const FArrayProperty* ArrayProperty = CastField<const FArrayProperty>(Property))
			{
				Definition.PropertyTypeName = ArrayProperty->Inner->GetClass()->GetFName();
				if (const FStructProperty* InnerStructProperty = CastField<const FStructProperty>(ArrayProperty->Inner))
				{
					Definition.ObjectTypeName = InnerStructProperty->Struct->GetFName();
				}
			}
			return Definition;
		}

		static FActActionAnimatedPropertyKey FromObjectType(const UClass* Class)
		{
			FActActionAnimatedPropertyKey Definition;
			Definition.PropertyTypeName = NAME_ObjectProperty;
			Definition.ObjectTypeName = Class->GetFName();
			return Definition;
		}

		static FActActionAnimatedPropertyKey FromStructType(const UStruct* Struct)
		{
			check(Struct);
			return FromStructType(Struct->GetFName());
		}

		static FActActionAnimatedPropertyKey FromStructType(FName StructName)
		{
			FActActionAnimatedPropertyKey Definition;
			Definition.PropertyTypeName = NAME_StructProperty;
			Definition.ObjectTypeName = StructName;
			return Definition;
		}

		static FActActionAnimatedPropertyKey FromPropertyTypeName(FName PropertyTypeName)
		{
			FActActionAnimatedPropertyKey Definition;
			Definition.PropertyTypeName = PropertyTypeName;
			return Definition;
		}

		static FActActionAnimatedPropertyKey FromPropertyType(TSubclassOf<FProperty> PropertyType)
		{
			FActActionAnimatedPropertyKey Definition;
			Definition.PropertyTypeName = PropertyType->GetFName();
			return Definition;
		}

	protected:
		FActActionAnimatedPropertyKey()
			: PropertyTypeName(NAME_None),
			  ObjectTypeName(NAME_None) { }
	};

	struct FActActionAnimatedTypeCache
	{
		FDelegateHandle FactoryHandle;

		TArray<FActActionAnimatedPropertyKey, TInlineAllocator<4>> AnimatedTypes;
	};

	struct FActActionPaintViewAreaArgs
	{
		/** Whether to display tick lines */
		bool bDisplayTickLines;

		/** Whether to display the scrub position */
		bool bDisplayScrubPosition;

		/** Whether to display the marked frames */
		bool bDisplayMarkedFrames;

		/** Optional Paint args for the playback range*/
		TOptional<FActActionPaintPlaybackRangeArgs> PlaybackRangeArgs;
	};

	/**
	* Sequence view parameters.
	*/
	struct FActActionSequenceViewParams
	{
		// OnGetAddMenuContentDelegate OnGetAddMenuContent;

		OnBuildCustomContextMenuForGuidDelegate OnBuildCustomContextMenuForGuid;

		/** Called when this sequencer has received user focus */
		FSimpleDelegate OnReceivedFocus;

		/** A menu extender for the add menu */
		TSharedPtr<FExtender> AddMenuExtender;

		/** A toolbar extender for the main toolbar */
		TSharedPtr<FExtender> ToolbarExtender;

		/** Unique name for the sequencer. */
		FString UniqueName;

		/** Whether the sequencer is read-only */
		bool bReadOnly;

		/** Style of scrubber to use */
		ENovaSequencerScrubberStyle ScrubberStyle;

		FActActionSequenceViewParams(FString InName = FString())
			: UniqueName(MoveTemp(InName)),
			  bReadOnly(false),
			  ScrubberStyle(ENovaSequencerScrubberStyle::Vanilla) { }
	};

	/** Structure used to define a column in the tree view */
	struct FActActionSequenceTreeViewColumn
	{
		typedef TFunction<TSharedRef<SWidget>(const TSharedRef<SActImageTreeViewTableRow>&, const TSharedRef<SActActionSequenceTreeViewRow>&)> FOnGenerate;

		FActActionSequenceTreeViewColumn(const FOnGenerate& InOnGenerate, const TAttribute<float>& InWidth)
			: Generator(InOnGenerate),
			  Width(InWidth) { }

		FActActionSequenceTreeViewColumn(FOnGenerate&& InOnGenerate, const TAttribute<float>& InWidth)
			: Generator(MoveTemp(InOnGenerate)),
			  Width(InWidth) { }

		/** Function used to generate a cell for this column */
		FOnGenerate Generator;

		/** Attribute specifying the width of this column */
		TAttribute<float> Width;
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
			  Direction(ENovaPlayDirection::Forwards),
			  TimeOverride(FFrameNumber(TNumericLimits<int32>::Lowest())) { }

		/**
		 * Construct this range from a raw range and a direction
		 */
		FActActionEvaluationRange(TRange<FFrameTime> InRange, FFrameRate InFrameRate, ENovaPlayDirection InDirection)
			: EvaluationRange(InRange),
			  CurrentFrameRate(InFrameRate),
			  Direction(InDirection),
			  TimeOverride(FFrameNumber(TNumericLimits<int32>::Lowest())) { }

		/**
		 * Construct this range from 2 times, and whether the range should include the previous time or not
		 */
		FActActionEvaluationRange(FFrameTime InCurrentTime, FFrameTime InPreviousTime, FFrameRate InFrameRate, bool bInclusivePreviousTime = false)
			: EvaluationRange(FActActionEvaluationRange::CalculateEvaluationRange(InCurrentTime, InPreviousTime, bInclusivePreviousTime)),
			  CurrentFrameRate(InFrameRate),
			  Direction((InCurrentTime - InPreviousTime >= FFrameTime()) ? ENovaPlayDirection::Forwards : ENovaPlayDirection::Backwards),
			  TimeOverride(TNumericLimits<int32>::Lowest()) { }

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
		FORCEINLINE ENovaPlayDirection GetDirection() const
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

			return Direction == ENovaPlayDirection::Forwards ? EvaluationRange.GetUpperBoundValue() : EvaluationRange.GetLowerBoundValue();
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
			return Direction == ENovaPlayDirection::Forwards ? EvaluationRange.GetLowerBoundValue() : EvaluationRange.GetUpperBoundValue();
		}

		/**
		 * Get the current time offset by the specified amount in the direction of play
		 */
		FORCEINLINE FFrameTime GetOffsetTime(FFrameTime InOffset) const
		{
			FFrameTime Now = GetTime();
			return Direction == ENovaPlayDirection::Forwards ? Now + InOffset : Now - InOffset;
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
		ENovaPlayDirection Direction;

		/** Overridden current time (Does not manipulate the actual evaluated range) */
		FFrameNumber TimeOverride;
	};

	/** Helper class designed to abstract the complexity of calculating evaluation ranges for previous times and fixed time intervals */
	struct FActActionPlaybackPosition
	{
		FActActionPlaybackPosition()
			: InputRate(0, 0),
			  OutputRate(0, 0) { }

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
			  StartTime(InStartTime) { }

		FFrameTime DestinationTime;
		FFrameTime SourceTime;
		double StartTime;
	};
}
