#pragma once
#include "ActActionDelegates.h"

class FActActionSequenceController;

class FActActionTrackEditorBase;

class FActActionSequenceTreeViewNode;

class SActActionSequenceTreeViewRow;

namespace ActActionSequence
{
	const FName ActActionSequenceTabId(TEXT("ActAction_Sequence"));

	const FName ActActionViewportTabId(TEXT("ActAction_Viewport"));

	const FName ActActionDetailsViewTabId(TEXT("ActAction_DetailsView"));


	/** Utility struct for converting between scrub range space and local/absolute screen space */
	struct FActActionScrubRangeToScreen
	{
		double ViewStart;

		float PixelsPerInput;

		FActActionScrubRangeToScreen(const TRange<double>& InViewInput, const FVector2D& InWidgetSize)
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
			  ClampRange(FActActionAnimatedRange(0.0f, 5.0f)),
			  AllowZoom(true)
		{
		}

		/** The scrub position */
		TAttribute<FFrameTime> ScrubPosition;

		/** The scrub position text */
		TAttribute<FString> ScrubPositionText;

		/** TimeSlider当前可见的区域 */
		TAttribute<FActActionAnimatedRange> ViewRange;

		/** TimeSlider可见区域的最大可调整的区域，与AnimSequence动画的播放区域相同 */
		TAttribute<FActActionAnimatedRange> ClampRange;

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

		// /** Attribute for the current sequence's vertical frames */
		// TAttribute<TSet<FFrameNumber>> VerticalFrames;

		/** Called when the marked frame needs to be set */
		// OnSetMarkedFrameDelegate OnSetMarkedFrame;

		/** Called when a marked frame is added */
		// OnAddMarkedFrameDelegate OnAddMarkedFrame;

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

	/** Structure used to define a column in the tree view */
	struct FActActionSequenceTreeViewColumn
	{
		typedef TFunction<TSharedRef<SWidget>(const TSharedRef<FActActionSequenceTreeViewNode>&, const TSharedRef<SActActionSequenceTreeViewRow>&)> FOnGenerate;

		FActActionSequenceTreeViewColumn(const FOnGenerate& InOnGenerate, const TAttribute<float>& InWidth)
			: Generator(InOnGenerate), Width(InWidth)
		{
		}

		FActActionSequenceTreeViewColumn(FOnGenerate&& InOnGenerate, const TAttribute<float>& InWidth)
			: Generator(MoveTemp(InOnGenerate)), Width(InWidth)
		{
		}

		/** Function used to generate a cell for this column */
		FOnGenerate Generator;

		/** Attribute specifying the width of this column */
		TAttribute<float> Width;
	};
}
