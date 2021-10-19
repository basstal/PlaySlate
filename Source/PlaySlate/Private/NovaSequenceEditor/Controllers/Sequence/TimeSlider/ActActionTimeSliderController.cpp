#include "ActActionTimeSliderController.h"

#include "ActActionSequenceSectionOverlayController.h"
#include "PlaySlate.h"
#include "Utils/ActActionStaticUtil.h"
#include "Utils/ActActionSequenceUtil.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/Subs/ActActionTimeRangeSlider.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/ActActionTimeSliderWidget.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/Subs/ActActionTimeRange.h"

FActActionTimeSliderController::FActActionTimeSliderController(const TSharedRef<FActActionSequenceController>& InSequenceController)
	: ActActionSequenceController(InSequenceController),
	  DistanceDragged(0),
	  MouseDragType(ActActionSequence::EDragType::DRAG_NONE),
	  bMouseDownInRegion(false),
	  bPanning(false),
	  bMirrorLabels(false)
{
}

FActActionTimeSliderController::~FActActionTimeSliderController()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionTimeSliderController::~FActActionTimeSliderController"));
}

void FActActionTimeSliderController::MakeTimeSliderWidget()
{
	// Create the top and bottom sliders
	ActActionTimeSliderWidget = SNew(SActActionTimeSliderWidget, SharedThis(this));
	TSharedRef<SActActionTimeRangeSlider> ActActionTimeRangeSlider = SNew(SActActionTimeRangeSlider, SharedThis(this));
	ActActionTimeRange = SNew(SActActionTimeRange, SharedThis(this), ActActionTimeRangeSlider);
	TickLinesSequenceSectionOverlayController = MakeShareable(new FActActionSequenceSectionOverlayController(SharedThis(this)));
	TickLinesSequenceSectionOverlayController->MakeSequenceSectionOverlayWidget(ActActionSequence::ESectionOverlayWidgetType::TickLines);
	ScrubPosSequenceSectionOverlayController = MakeShareable(new FActActionSequenceSectionOverlayController(SharedThis(this)));
	ScrubPosSequenceSectionOverlayController->MakeSequenceSectionOverlayWidget(ActActionSequence::ESectionOverlayWidgetType::ScrubPosition);
}

FFrameTime FActActionTimeSliderController::ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	const double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);
	return MouseValue * TimeSliderArgs.TickResolution.Get();
}

FFrameTime FActActionTimeSliderController::ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	const double MouseSeconds = RangeToScreen.LocalXToInput(CursorPos.X);
	const FFrameTime ScrubTime = MouseSeconds * TimeSliderArgs.TickResolution.Get();
	return ScrubTime;
}

void FActActionTimeSliderController::CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing) const
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	// The user can scrub past the viewing range of the time slider controller, so we clamp it to the view range.
	const ActActionSequence::FActActionAnimatedRange ViewRange = TimeSliderArgs.ViewRange.Get();
	const FFrameRate TickResolution = ActActionSequenceController.Pin()->GetActActionSequenceEditor()->GetTickResolution();
	const FFrameTime LowerBound = (ViewRange.GetLowerBoundValue() * TickResolution).CeilToFrame();
	const FFrameTime UpperBound = (ViewRange.GetUpperBoundValue() * TickResolution).FloorToFrame();
	NewValue = FMath::Clamp(NewValue, LowerBound, UpperBound);
	// Manage the scrub position ourselves if its not bound to a delegate
	if (!TimeSliderArgs.ScrubPosition.IsBound())
	{
		TimeSliderArgs.ScrubPosition.Set(NewValue);
	}
	TimeSliderArgs.OnScrubPositionChanged.ExecuteIfBound(NewValue, bIsScrubbing);
}

void FActActionTimeSliderController::SetViewRange(double NewRangeMin, double NewRangeMax, ActActionSequence::EActActionViewRangeInterpolation Interpolation) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	// Clamp to a minimum size to avoid zero-sized or negative visible ranges
	const double MinVisibleTimeRange = FFrameNumber(1) / TimeSliderArgs.TickResolution.Get();
	const TRange<double> ExistingViewRange = TimeSliderArgs.ViewRange.Get();

	if (NewRangeMax == ExistingViewRange.GetUpperBoundValue())
	{
		if (NewRangeMin > NewRangeMax - MinVisibleTimeRange)
		{
			NewRangeMin = NewRangeMax - MinVisibleTimeRange;
		}
	}
	else if (NewRangeMax < NewRangeMin + MinVisibleTimeRange)
	{
		NewRangeMax = NewRangeMin + MinVisibleTimeRange;
	}

	// Clamp to the clamp range
	const TRange<double> NewRange = TRange<double>(NewRangeMin, NewRangeMax);
	UE_LOG(LogActAction, Log, TEXT("NewRangeMin : %f, NewRangeMax : %f"), NewRangeMin, NewRangeMax);

	TimeSliderArgs.OnViewRangeChanged.ExecuteIfBound(NewRange, Interpolation);
}

bool FActActionTimeSliderController::HitTestRangeStart(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeStartPixel = RangeToScreen.InputToLocalX(Range.GetLowerBoundValue());

	// Hit test against the brush region to the right of the playback start position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeStartPixel - 4.0f && HitPixel <= RangeStartPixel + 10.0f;
}

bool FActActionTimeSliderController::HitTestRangeEnd(const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeEndPixel = RangeToScreen.InputToLocalX(Range.GetUpperBoundValue());
	// Hit test against the brush region to the left of the playback end position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeEndPixel - 10.0f && HitPixel <= RangeEndPixel + 4.0f;
}

void FActActionTimeSliderController::SetPlaybackRangeStart(FFrameNumber NewStart) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(PlaybackRange.GetUpperBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, PlaybackRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetPlaybackRangeEnd(FFrameNumber NewEnd) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(PlaybackRange.GetLowerBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(PlaybackRange.GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive(NewEnd)));
	}
}

void FActActionTimeSliderController::SetSelectionRangeStart(FFrameNumber NewStart) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, NewStart + 1));
	}
	else if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(SelectionRange.GetUpperBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, SelectionRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetSelectionRangeEnd(FFrameNumber NewEnd) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewEnd - 1, NewEnd));
	}
	else if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(SelectionRange.GetLowerBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(SelectionRange.GetLowerBound(), NewEnd));
	}
}

bool FActActionTimeSliderController::ZoomByDelta(float InDelta, float ZoomBias) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();
	const double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	const double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const double OutputViewSize = LocalViewRangeMax - LocalViewRangeMin;
	const double OutputChange = OutputViewSize * InDelta;

	const double NewViewOutputMin = LocalViewRangeMin - (OutputChange * ZoomBias);
	const double NewViewOutputMax = LocalViewRangeMax + (OutputChange * (1.f - ZoomBias));

	if (NewViewOutputMin < NewViewOutputMax)
	{
		SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
		return true;
	}

	return false;
}

void FActActionTimeSliderController::PanByDelta(float InDelta) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();

	const double CurrentMin = LocalViewRange.GetLowerBoundValue();
	const double CurrentMax = LocalViewRange.GetUpperBoundValue();

	// Adjust the delta to be a percentage of the current range
	InDelta *= 0.1f * (CurrentMax - CurrentMin);

	const double NewViewOutputMin = CurrentMin + InDelta;
	const double NewViewOutputMax = CurrentMax + InDelta;

	SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
}

FReply FActActionTimeSliderController::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
	DistanceDragged = 0;
	MouseDownPosition[0] = MouseDownPosition[1] = MouseEvent.GetScreenSpacePosition();
	bMouseDownInRegion = false;
	MouseDownGeometry = MyGeometry;

	const FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();
	const FVector2D LocalPos = MouseDownGeometry.AbsoluteToLocal(CursorPos);
	if (LocalPos.Y >= 0 && LocalPos.Y < MouseDownGeometry.GetLocalSize().Y)
	{
		bMouseDownInRegion = true;
	}

	return FReply::Unhandled();
}

FReply FActActionTimeSliderController::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	const bool bHandleLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ActActionTimeSliderWidget->HasMouseCapture();
	const bool bHandleRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ActActionTimeSliderWidget->HasMouseCapture() && TimeSliderArgs.AllowZoom;
	const ActActionSequence::FActActionScrubRangeToScreen RangeToScreen = ActActionSequence::FActActionScrubRangeToScreen(TimeSliderArgs.ViewRange.Get(), MyGeometry.Size);
	const FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

	if (bHandleRightMouseButton)
	{
		if (!bPanning)
		{
			// return unhandled in case our parent wants to use our right mouse button to open a context menu
			if (DistanceDragged == 0.0f)
			{
				return FReply::Unhandled().ReleaseMouseCapture();
			}
		}

		bPanning = false;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}
	else if (bHandleLeftMouseButton)
	{
		if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_START)
		{
			TimeSliderArgs.OnPlaybackRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_END)
		{
			TimeSliderArgs.OnPlaybackRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_START)
		{
			TimeSliderArgs.OnSelectionRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_END)
		{
			TimeSliderArgs.OnSelectionRangeEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_MARK)
		{
			TimeSliderArgs.OnMarkEndDrag.ExecuteIfBound();
		}
		else if (MouseDragType == ActActionSequence::EDragType::DRAG_SETTING_RANGE)
		{
			// Zooming
			const FFrameTime MouseDownStart = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen);

			const bool bCanZoomIn = MouseTime > MouseDownStart;
			const bool bCanZoomOut = ViewRangeStack.Num() > 0;
			if (bCanZoomIn || bCanZoomOut)
			{
				TRange<double> ViewRange = TimeSliderArgs.ViewRange.Get();
				if (!bCanZoomIn)
				{
					ViewRange = ViewRangeStack.Pop();
				}

				if (bCanZoomIn)
				{
					// push the current value onto the stack
					ViewRangeStack.Add(ViewRange);

					ViewRange = TRange<double>(MouseDownStart.FrameNumber / TimeSliderArgs.TickResolution.Get(), MouseTime.FrameNumber / TimeSliderArgs.TickResolution.Get());
				}

				TimeSliderArgs.OnViewRangeChanged.ExecuteIfBound(ViewRange, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
			}
		}
		else if (bMouseDownInRegion)
		{
			TimeSliderArgs.OnEndScrubberMovement.ExecuteIfBound();

			FFrameTime ScrubTime = MouseTime;
			const FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();

			if (MouseDragType == ActActionSequence::EDragType::DRAG_SCRUBBING_TIME)
			{
				ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, CursorPos, RangeToScreen);
			}

			CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/false);
		}

		MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
		DistanceDragged = 0.0f;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}

	bMouseDownInRegion = false;
	return FReply::Unhandled();
}

FReply FActActionTimeSliderController::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();

	bool bHandleLeftMouseButton = MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bHandleRightMouseButton = MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && TimeSliderArgs.AllowZoom;

	if (bHandleRightMouseButton)
	{
		if (!bPanning)
		{
			DistanceDragged += FMath::Abs(MouseEvent.GetCursorDelta().X);
			if (DistanceDragged > FSlateApplication::Get().GetDragTriggerDistance())
			{
				bPanning = true;
			}
		}
		else
		{
			TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
			double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
			double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();

			ActActionSequence::FActActionScrubRangeToScreen ScaleInfo(LocalViewRange, MyGeometry.Size);
			FVector2D ScreenDelta = MouseEvent.GetCursorDelta();
			FVector2D InputDelta;
			InputDelta.X = ScreenDelta.X / ScaleInfo.PixelsPerInput;

			double NewViewOutputMin = LocalViewRangeMin - InputDelta.X;
			double NewViewOutputMax = LocalViewRangeMax - InputDelta.X;

			SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Immediate);
		}
	}
	else if (bHandleLeftMouseButton)
	{
		TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get();
		ActActionSequence::FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, MyGeometry.Size);
		DistanceDragged += FMath::Abs(MouseEvent.GetCursorDelta().X);

		if (MouseDragType == ActActionSequence::EDragType::DRAG_NONE)
		{
			if (DistanceDragged > FSlateApplication::Get().GetDragTriggerDistance())
			{
				FFrameTime MouseDownFree = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen, false);

				const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
				const FFrameRate DisplayRate = TimeSliderArgs.DisplayRate.Get();
				const FQualifiedFrameTime QualifiedFrameTime = FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), TickResolution);
				const bool bLockedPlayRange = TimeSliderArgs.IsPlaybackRangeLocked.Get();
				const float MouseDownPixel = RangeToScreen.InputToLocalX(MouseDownFree / TickResolution);
				ActActionSequence::FActActionScrubberMetrics ScrubberMetrics = ActActionSequence::ActActionStaticUtil::GetScrubPixelMetrics(DisplayRate, QualifiedFrameTime, RangeToScreen, 4.0f);
				const bool bHitScrubber = ScrubberMetrics.HandleRangePx.Contains(MouseDownPixel);

				TRange<double> SelectionRange = TimeSliderArgs.SelectionRange.Get() / TickResolution;
				TRange<double> PlaybackRange = TimeSliderArgs.PlaybackRange.Get() / TickResolution;

				// Disable selection range test if it's empty so that the playback range scrubbing gets priority
				if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeEnd(RangeToScreen, SelectionRange, MouseDownPixel))
				{
					// selection range end scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_SELECTION_END;
					TimeSliderArgs.OnSelectionRangeBeginDrag.ExecuteIfBound();
				}
				else if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeStart(RangeToScreen, SelectionRange, MouseDownPixel))
				{
					// selection range start scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_SELECTION_START;
					TimeSliderArgs.OnSelectionRangeBeginDrag.ExecuteIfBound();
				}
				else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeEnd(RangeToScreen, PlaybackRange, MouseDownPixel))
				{
					// playback range end scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_PLAYBACK_END;
					TimeSliderArgs.OnPlaybackRangeBeginDrag.ExecuteIfBound();
				}
				else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeStart(RangeToScreen, PlaybackRange, MouseDownPixel))
				{
					// playback range start scrubber
					MouseDragType = ActActionSequence::EDragType::DRAG_PLAYBACK_START;
					TimeSliderArgs.OnPlaybackRangeBeginDrag.ExecuteIfBound();
				}
				else if (FSlateApplication::Get().GetModifierKeys().AreModifersDown(EModifierKey::Control))
				{
					MouseDragType = ActActionSequence::EDragType::DRAG_SETTING_RANGE;
				}
				else if (bMouseDownInRegion)
				{
					MouseDragType = ActActionSequence::EDragType::DRAG_SCRUBBING_TIME;
					TimeSliderArgs.OnBeginScrubberMovement.ExecuteIfBound();
				}
			}
		}
		else
		{
			FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);
			FFrameTime ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

			// Set the start range time?
			if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_START)
			{
				SetPlaybackRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_PLAYBACK_END)
			{
				SetPlaybackRangeEnd(MouseTime.FrameNumber);
			}
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_START)
			{
				SetSelectionRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SELECTION_END)
			{
				SetSelectionRangeEnd(MouseTime.FrameNumber);
			}
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SCRUBBING_TIME)
			{
				// Delegate responsibility for clamping to the current view range to the client
				CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/true);
			}
			else if (MouseDragType == ActActionSequence::EDragType::DRAG_SETTING_RANGE)
			{
				MouseDownPosition[1] = MouseEvent.GetScreenSpacePosition();
			}
		}
	}

	if (DistanceDragged != 0.0f && (bHandleLeftMouseButton || bHandleRightMouseButton))
	{
		return FReply::Handled().CaptureMouse(ActActionTimeSliderWidget.ToSharedRef());
	}


	return FReply::Handled();
}

FReply FActActionTimeSliderController::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	const ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TOptional<TRange<float>> NewTargetRange;

	if (TimeSliderArgs.AllowZoom && MouseEvent.IsControlDown())
	{
		const float MouseFractionX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / MyGeometry.GetLocalSize().X;

		const float ZoomDelta = -0.2f * MouseEvent.GetWheelDelta();
		if (ZoomByDelta(ZoomDelta, MouseFractionX))
		{
			return FReply::Handled();
		}
	}
	else if (MouseEvent.IsShiftDown())
	{
		PanByDelta(-MouseEvent.GetWheelDelta());
		return FReply::Handled();
	}

	return FReply::Unhandled();
}

ActActionSequence::FActActionTimeSliderArgs& FActActionTimeSliderController::GetTimeSliderArgs() const
{
	return ActActionSequenceController.Pin()->GetTimeSliderArgs();
}

int32 FActActionTimeSliderController::DrawSubSequenceRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const
{
	const ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs = GetTimeSliderArgs();
	TOptional<TRange<FFrameNumber>> RangeValue;
	RangeValue = TimeSliderArgs.SubSequenceRange.Get(RangeValue);

	if (!RangeValue.IsSet() || RangeValue->IsEmpty())
	{
		return LayerId;
	}

	const FFrameRate Resolution = TimeSliderArgs.TickResolution.Get();
	const FFrameNumber LowerFrame = RangeValue.GetValue().GetLowerBoundValue();
	const FFrameNumber UpperFrame = RangeValue.GetValue().GetUpperBoundValue();
	const float SubSequenceRangeL = RangeToScreen.InputToLocalX(LowerFrame / Resolution) - 1;
	const float SubSequenceRangeR = RangeToScreen.InputToLocalX(UpperFrame / Resolution) + 1;
	static const FSlateBrush* LineBrushL(FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_L"));
	static const FSlateBrush* LineBrushR(FEditorStyle::GetBrush("Sequencer.Timeline.PlayRange_R"));
	constexpr FColor GreenTint(32, 128, 32); // 120, 75, 50 (HSV)

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeL, 0.0f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		LineBrushL,
		ESlateDrawEffect::None,
		GreenTint
	);

	constexpr FColor RedTint(128, 32, 32); // 0, 75, 50 (HSV)
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR - Args.BrushWidth, 0.0f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		LineBrushR,
		ESlateDrawEffect::None,
		RedTint
	);

	// Black tint for excluded regions
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(0.0f, 0.0f), FVector2D(SubSequenceRangeL, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR, 0.0f), FVector2D(AllottedGeometry.Size.X - SubSequenceRangeR, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f)
	);

	// Hash applied to the left and right of the sequence bounds
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeL - 16.f, 0.0f), FVector2D(16.f, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("Sequencer.Timeline.SubSequenceRangeHashL"),
		ESlateDrawEffect::None,
		GreenTint
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(SubSequenceRangeR, 0.0f), FVector2D(16.f, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("Sequencer.Timeline.SubSequenceRangeHashR"),
		ESlateDrawEffect::None,
		RedTint
	);

	return LayerId + 1;
}

int32 FActActionTimeSliderController::DrawPlaybackRange(const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionPaintPlaybackRangeArgs& Args) const
{
	const ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs = GetTimeSliderArgs();
	if (!TimeSliderArgs.PlaybackRange.IsSet())
	{
		return LayerId;
	}

	const uint8 OpacityBlend = TimeSliderArgs.SubSequenceRange.Get().IsSet() ? 128 : 255;

	const TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();
	const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	const float PlaybackRangeL = RangeToScreen.InputToLocalX(PlaybackRange.GetLowerBoundValue() / TickResolution);
	const float PlaybackRangeR = RangeToScreen.InputToLocalX(PlaybackRange.GetUpperBoundValue() / TickResolution) - 1;

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeL, 0.0f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		Args.StartBrush,
		ESlateDrawEffect::None,
		FColor(32, 128, 32, OpacityBlend) // 120, 75, 50 (HSV)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeR - Args.BrushWidth, 0.0f), FVector2D(Args.BrushWidth, AllottedGeometry.Size.Y)),
		Args.EndBrush,
		ESlateDrawEffect::None,
		FColor(128, 32, 32, OpacityBlend) // 0, 75, 50 (HSV)
	);

	// Black tint for excluded regions
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(0.0f, 0.0f), FVector2D(PlaybackRangeL, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f * OpacityBlend / 255.f)
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId + 1,
		AllottedGeometry.ToPaintGeometry(FVector2D(PlaybackRangeR, 0.0f), FVector2D(AllottedGeometry.Size.X - PlaybackRangeR, AllottedGeometry.Size.Y)),
		FEditorStyle::GetBrush("WhiteBrush"),
		ESlateDrawEffect::None,
		FLinearColor::Black.CopyWithNewOpacity(0.3f * OpacityBlend / 255.f)
	);

	return LayerId + 1;
}

void FActActionTimeSliderController::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<double>& ViewRange, const ActActionSequence::FActActionScrubRangeToScreen& RangeToScreen, const ActActionSequence::FActActionDrawTickArgs& InArgs) const
{
	const ActActionSequence::FActActionTimeSliderArgs TimeSliderArgs = GetTimeSliderArgs();
	const FFrameRate TickResolution = TimeSliderArgs.TickResolution.Get();
	const FFrameRate DisplayRate = TimeSliderArgs.DisplayRate.Get();
	const FPaintGeometry PaintGeometry = InArgs.AllottedGeometry.ToPaintGeometry();
	const FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

	double MajorGridStep = 0.0;
	int32 MinorDivisions = 0;
	if (!ActActionSequence::ActActionStaticUtil::GetGridMetrics(TimeSliderArgs.NumericTypeInterface, InArgs.AllottedGeometry.Size.X, ViewRange.GetLowerBoundValue(), ViewRange.GetUpperBoundValue(), MajorGridStep, MinorDivisions))
	{
		return;
	}

	if (InArgs.bOnlyDrawMajorTicks)
	{
		MinorDivisions = 0;
	}

	TArray<FVector2D> LinePoints;
	LinePoints.SetNumUninitialized(2);

	const double FirstMajorLine = FMath::FloorToDouble(ViewRange.GetLowerBoundValue() / MajorGridStep) * MajorGridStep;
	const double LastMajorLine = FMath::CeilToDouble(ViewRange.GetUpperBoundValue() / MajorGridStep) * MajorGridStep;
	const FFrameNumber FrameNumber = ConvertFrameTime(TimeSliderArgs.ScrubPosition.Get(), TickResolution, DisplayRate).FloorToFrame();
	const float FlooredScrubPx = RangeToScreen.InputToLocalX(FrameNumber / DisplayRate);
	for (double CurrentMajorLine = FirstMajorLine; CurrentMajorLine < LastMajorLine; CurrentMajorLine += MajorGridStep)
	{
		const float MajorLinePx = RangeToScreen.InputToLocalX(CurrentMajorLine);

		LinePoints[0] = FVector2D(MajorLinePx, InArgs.TickOffset);
		LinePoints[1] = FVector2D(MajorLinePx, InArgs.TickOffset + InArgs.MajorTickHeight);

		// Draw each tick mark
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			InArgs.StartLayer,
			PaintGeometry,
			LinePoints,
			InArgs.DrawEffects,
			InArgs.TickColor,
			false
		);

		if (!InArgs.bOnlyDrawMajorTicks && !FMath::IsNearlyEqual(MajorLinePx, FlooredScrubPx, 3.f))
		{
			FString FrameString = TimeSliderArgs.NumericTypeInterface->ToString((CurrentMajorLine * TickResolution).RoundToFrame().Value);

			// Space the text between the tick mark but slightly above
			FVector2D TextOffset(MajorLinePx + 5.f, InArgs.bMirrorLabels ? 1.f : FMath::Abs(InArgs.AllottedGeometry.Size.Y - (InArgs.MajorTickHeight + 3.f)));
			FSlateDrawElement::MakeText(
				OutDrawElements,
				InArgs.StartLayer + 1,
				InArgs.AllottedGeometry.ToPaintGeometry(TextOffset, InArgs.AllottedGeometry.Size),
				FrameString,
				SmallLayoutFont,
				InArgs.DrawEffects,
				InArgs.TickColor * 0.65f
			);
		}

		for (int32 Step = 1; Step < MinorDivisions; ++Step)
		{
			// Compute the size of each tick mark.  If we are half way between to visible values display a slightly larger tick mark
			const float MinorTickHeight = ((MinorDivisions % 2 == 0) && (Step % (MinorDivisions / 2)) == 0) ? 6.0f : 2.0f;
			const float MinorLinePx = RangeToScreen.InputToLocalX(CurrentMajorLine + Step * MajorGridStep / MinorDivisions);

			LinePoints[0] = FVector2D(MinorLinePx, InArgs.bMirrorLabels ? 0.0f : FMath::Abs(InArgs.AllottedGeometry.Size.Y - MinorTickHeight));
			LinePoints[1] = FVector2D(MinorLinePx, LinePoints[0].Y + MinorTickHeight);

			// Draw each sub mark
			FSlateDrawElement::MakeLines(
				OutDrawElements,
				InArgs.StartLayer,
				PaintGeometry,
				LinePoints,
				InArgs.DrawEffects,
				InArgs.TickColor,
				false
			);
		}
	}
}
