#pragma once

class FActActionSequenceController;
class FActActionTrackEditorBase;

namespace ActActionSequence
{
	const FName ActActionSequenceTabId(TEXT("ActAction_Sequence"));
	const FName ActActionViewportTabId(TEXT("ActAction_Viewport"));

	enum class ESequenceNodeType : uint8
	{
		Root,
		Folder,
		Object,
		Track,
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

	/** A delegate which will create an auto-key handler. */
	DECLARE_DELEGATE_RetVal_TwoParams(FFrameNumber, OnGetNearestKeyDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FActActionTrackEditorBase>, OnCreateTrackEditorDelegate, TSharedRef<FActActionSequenceController>);

	/** A delegate that is executed when menu object is clicked. Unlike FExtender delegates we pass in the FGuid which exists even for deleted objects. */
	DECLARE_DELEGATE_TwoParams(OnBuildCustomContextMenuForGuidDelegate, FMenuBuilder&, FGuid);
	DECLARE_DELEGATE_TwoParams(OnScrubPositionChangedDelegate, FFrameTime, bool)
	DECLARE_DELEGATE_TwoParams(OnViewRangeChangedDelegate, TRange<double>, EActActionViewRangeInterpolation)
	/** A delegate that is executed when adding menu content. */
	DECLARE_DELEGATE_TwoParams(OnGetAddMenuContentDelegate, FMenuBuilder& /*MenuBuilder*/, TSharedRef<FActActionSequenceController>);
	DECLARE_DELEGATE_TwoParams(OnSetMarkedFrameDelegate, int32, FFrameNumber)
	DECLARE_DELEGATE_OneParam(OnTimeRangeChangedDelegate, TRange<double>)
	DECLARE_DELEGATE_OneParam(OnFrameRangeChangedDelegate, TRange<FFrameNumber>)
	DECLARE_DELEGATE_OneParam(OnAddMarkedFrameDelegate, FFrameNumber)
	DECLARE_DELEGATE_OneParam(OnDeleteMarkedFrameDelegate, int32)
	DECLARE_DELEGATE_OneParam(OnGetContextMenuContentDelegate, FMenuBuilder&);
	/** Called when an asset is selected in the asset view */
	DECLARE_DELEGATE_OneParam(OnAssetSelectedDelegate, const FAssetData& /*AssetData*/);
	/** Called when enter is pressed on an asset in the asset view */
	DECLARE_DELEGATE_OneParam(OnAssetEnterPressedDelegate, const TArray<FAssetData>& /*SelectedAssets*/);

	/** Utility struct for converting between scrub range space and local/absolute screen space */
	struct FActActionScrubRangeToScreen
	{
		double ViewStart;
		float PixelsPerInput;

		FActActionScrubRangeToScreen(const TRange<double>& InViewInput, const FVector2D& InWidgetSize)
		{
			float ViewInputRange = InViewInput.Size<double>();

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

	/** Structure used to wrap up a range, and an optional animation target */
	struct FActActionAnimatedRange : public TRange<double>
	{
		FActActionAnimatedRange()
			: TRange()
		{
		}

		FActActionAnimatedRange(double LowerBound, double UpperBound)
			: TRange(LowerBound, UpperBound)
		{
		}

		FActActionAnimatedRange(const TRange<double>& InRange)
			: TRange(InRange)
		{
		}

		/** Helper function to wrap an attribute to an animated range with a non-animated one */
		static TAttribute<TRange<double>> WrapAttribute(const TAttribute<FActActionAnimatedRange>& InAttribute)
		{
			return TAttribute<TRange<double>>::Create(TAttribute<TRange<double>>::FGetter::CreateLambda([=]() { return InAttribute.Get(); }));
		}

		/** Helper function to wrap an attribute to a non-animated range with an animated one */
		static TAttribute<FActActionAnimatedRange> WrapAttribute(const TAttribute<TRange<double>>& InAttribute)
		{
			return TAttribute<FActActionAnimatedRange>::Create(TAttribute<FActActionAnimatedRange>::FGetter::CreateLambda([=]() { return InAttribute.Get(); }));
		}

		/** Get the current animation target, or the whole view range when not animating */
		const TRange<double>& GetAnimationTarget() const
		{
			return AnimationTarget.IsSet() ? AnimationTarget.GetValue() : *this;
		}

		/** The animation target, if animating */
		TOptional<TRange<double>> AnimationTarget;
	};

	struct FActActionTimeSliderArgs
	{
		FActActionTimeSliderArgs()
			: ScrubPosition(0),
			  ViewRange(FActActionAnimatedRange(0.0f, 5.0f)),
			  AllowZoom(true)
		{
		}

		/** The scrub position */
		TAttribute<FFrameTime> ScrubPosition;

		/** The scrub position text */
		TAttribute<FString> ScrubPositionText;

		/** View time range */
		TAttribute<FActActionAnimatedRange> ViewRange;

		/** Called when the scrub position changes */
		OnScrubPositionChangedDelegate OnScrubPositionChanged;

		/** Called right before the scrubber begins to move */
		FSimpleDelegate OnBeginScrubberMovement;

		/** Called right after the scrubber handle is released by the user */
		FSimpleDelegate OnEndScrubberMovement;

		/** Called when the view range changes */
		OnViewRangeChangedDelegate OnViewRangeChanged;

		// /** Called when the clamp range changes */
		// OnTimeRangeChangedDelegate OnClampRangeChanged;

		/** Delegate that is called when getting the nearest key */
		OnGetNearestKeyDelegate OnGetNearestKey;

		/** Attribute defining the active sub-sequence range for this controller */
		TAttribute<TOptional<TRange<FFrameNumber>>> SubSequenceRange;

		/** Attribute defining the playback range for this controller */
		TAttribute<TRange<FFrameNumber>> PlaybackRange;

		/** Attribute for the current sequence's display rate */
		TAttribute<FFrameRate> DisplayRate;

		/** Attribute for the current sequence's tick resolution */
		TAttribute<FFrameRate> TickResolution;

		/**
		 * 动画播放区间改变的回调，将数据写入到Model中保存
		 * Delegate that is called when the playback range wants to change
		 */
		OnFrameRangeChangedDelegate OnPlaybackRangeChanged;

		/** Called right before the playback range starts to be dragged */
		FSimpleDelegate OnPlaybackRangeBeginDrag;

		/** Called right after the playback range has finished being dragged */
		FSimpleDelegate OnPlaybackRangeEndDrag;

		/** Attribute defining the selection range for this controller */
		TAttribute<TRange<FFrameNumber>> SelectionRange;

		/** Delegate that is called when the selection range wants to change */
		OnFrameRangeChangedDelegate OnSelectionRangeChanged;

		/** Called right before the selection range starts to be dragged */
		FSimpleDelegate OnSelectionRangeBeginDrag;

		/** Called right after the selection range has finished being dragged */
		FSimpleDelegate OnSelectionRangeEndDrag;

		/** Called right before a mark starts to be dragged */
		FSimpleDelegate OnMarkBeginDrag;

		/** Called right after a mark has finished being dragged */
		FSimpleDelegate OnMarkEndDrag;

		/** Attribute for the current sequence's vertical frames */
		TAttribute<TSet<FFrameNumber>> VerticalFrames;

		/** Called when the marked frame needs to be set */
		OnSetMarkedFrameDelegate OnSetMarkedFrame;

		/** Called when a marked frame is added */
		OnAddMarkedFrameDelegate OnAddMarkedFrame;

		/** Called when a marked frame is deleted */
		OnDeleteMarkedFrameDelegate OnDeleteMarkedFrame;

		/** Called when all marked frames should be deleted */
		FSimpleDelegate OnDeleteAllMarkedFrames;

		/** Round the scrub position to an integer during playback */
		TAttribute<EPlaybackType> PlaybackStatus;

		/** Attribute defining whether the playback range is locked */
		TAttribute<bool> IsPlaybackRangeLocked;

		/** Attribute defining the time snap interval */
		TAttribute<float> TimeSnapInterval;

		/** Called when toggling the playback range lock */
		FSimpleDelegate OnTogglePlaybackRangeLocked;

		/** If we are allowed to zoom */
		bool AllowZoom;

		/** Numeric Type interface for converting between frame numbers and display formats. */
		TSharedPtr<INumericTypeInterface<double>> NumericTypeInterface;
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
		ESequencerScrubberStyle Style;
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
			  ObjectTypeName(NAME_None)
		{
		}
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
		OnGetAddMenuContentDelegate OnGetAddMenuContent;

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
		ESequencerScrubberStyle ScrubberStyle;

		FActActionSequenceViewParams(FString InName = FString())
			: UniqueName(MoveTemp(InName)),
			  bReadOnly(false),
			  ScrubberStyle(ESequencerScrubberStyle::Vanilla)
		{
		}
	};
}
