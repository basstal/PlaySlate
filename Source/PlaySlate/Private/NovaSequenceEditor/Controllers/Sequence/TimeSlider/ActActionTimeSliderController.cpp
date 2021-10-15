#include "ActActionTimeSliderController.h"

#include "PlaySlate.h"
#include "Utils/ActActionStaticUtil.h"
#include "Utils/ActActionSequenceUtil.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Widgets/Sequence/TimeSlider/ActActionTimeSliderWidget.h"

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
}

FFrameTime FActActionTimeSliderController::ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping) const
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);
	return MouseValue * TimeSliderArgs.TickResolution.Get();
}

FFrameTime FActActionTimeSliderController::ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, ActActionSequence::FActActionScrubRangeToScreen RangeToScreen) const
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	double MouseSeconds = RangeToScreen.LocalXToInput(CursorPos.X);
	FFrameTime ScrubTime = MouseSeconds * TimeSliderArgs.TickResolution.Get();
	return ScrubTime;
}

void FActActionTimeSliderController::CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	// The user can scrub past the viewing range of the time slider controller, so we clamp it to the view range.
	ActActionSequence::FActActionAnimatedRange ViewRange = TimeSliderArgs.ViewRange.Get();
	FFrameRate TickResolution = ActActionSequenceController.Pin()->GetActActionSequenceEditor()->GetTickResolution();
	FFrameTime LowerBound = (ViewRange.GetLowerBoundValue() * TickResolution).CeilToFrame();
	FFrameTime UpperBound = (ViewRange.GetUpperBoundValue() * TickResolution).FloorToFrame();
	NewValue = FMath::Clamp(NewValue, LowerBound, UpperBound);
	// Manage the scrub position ourselves if its not bound to a delegate
	if (!TimeSliderArgs.ScrubPosition.IsBound())
	{
		TimeSliderArgs.ScrubPosition.Set(NewValue);
	}
	TimeSliderArgs.OnScrubPositionChanged.ExecuteIfBound(NewValue, bIsScrubbing);
}

void FActActionTimeSliderController::SetViewRange(double NewRangeMin, double NewRangeMax, ActActionSequence::EActActionViewRangeInterpolation Interpolation)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	// Clamp to a minimum size to avoid zero-sized or negative visible ranges
	double MinVisibleTimeRange = FFrameNumber(1) / TimeSliderArgs.TickResolution.Get();
	TRange<double> ExistingViewRange = TimeSliderArgs.ViewRange.Get();

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

	TimeSliderArgs.OnViewRangeChanged.ExecuteIfBound(NewRange, Interpolation);
	if (!TimeSliderArgs.ViewRange.IsBound())
	{
		// The  output is not bound to a delegate so we'll manage the value ourselves (no animation)
		TimeSliderArgs.ViewRange.Set(NewRange);
	}
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

void FActActionTimeSliderController::SetPlaybackRangeStart(FFrameNumber NewStart)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(PlaybackRange.GetUpperBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, PlaybackRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetPlaybackRangeEnd(FFrameNumber NewEnd)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<FFrameNumber> PlaybackRange = TimeSliderArgs.PlaybackRange.Get();

	if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(PlaybackRange.GetLowerBound()))
	{
		TimeSliderArgs.OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(PlaybackRange.GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive(NewEnd)));
	}
}

void FActActionTimeSliderController::SetSelectionRangeStart(FFrameNumber NewStart)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, NewStart + 1));
	}
	else if (NewStart <= ActActionSequence::ActActionStaticUtil::DiscreteExclusiveUpper(SelectionRange.GetUpperBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, SelectionRange.GetUpperBound()));
	}
}

void FActActionTimeSliderController::SetSelectionRangeEnd(FFrameNumber NewEnd)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<FFrameNumber> SelectionRange = TimeSliderArgs.SelectionRange.Get();

	if (SelectionRange.IsEmpty())
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewEnd - 1, NewEnd));
	}
	else if (NewEnd >= ActActionSequence::ActActionStaticUtil::DiscreteInclusiveLower(SelectionRange.GetLowerBound()))
	{
		TimeSliderArgs.OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(SelectionRange.GetLowerBound(), NewEnd));
	}
}

bool FActActionTimeSliderController::ZoomByDelta(float InDelta, float ZoomBias)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();
	double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const double OutputViewSize = LocalViewRangeMax - LocalViewRangeMin;
	const double OutputChange = OutputViewSize * InDelta;

	double NewViewOutputMin = LocalViewRangeMin - (OutputChange * ZoomBias);
	double NewViewOutputMax = LocalViewRangeMax + (OutputChange * (1.f - ZoomBias));

	if (NewViewOutputMin < NewViewOutputMax)
	{
		SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
		return true;
	}

	return false;
}

void FActActionTimeSliderController::PanByDelta(float InDelta)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TRange<double> LocalViewRange = TimeSliderArgs.ViewRange.Get().GetAnimationTarget();

	double CurrentMin = LocalViewRange.GetLowerBoundValue();
	double CurrentMax = LocalViewRange.GetUpperBoundValue();

	// Adjust the delta to be a percentage of the current range
	InDelta *= 0.1f * (CurrentMax - CurrentMin);

	double NewViewOutputMin = CurrentMin + InDelta;
	double NewViewOutputMax = CurrentMax + InDelta;

	SetViewRange(NewViewOutputMin, NewViewOutputMax, ActActionSequence::EActActionViewRangeInterpolation::Animated);
}


FReply FActActionTimeSliderController::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
	DistanceDragged = 0;
	MouseDownPosition[0] = MouseDownPosition[1] = MouseEvent.GetScreenSpacePosition();
	bMouseDownInRegion = false;
	MouseDownGeometry = MyGeometry;

	FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();
	FVector2D LocalPos = MouseDownGeometry.AbsoluteToLocal(CursorPos);
	if (LocalPos.Y >= 0 && LocalPos.Y < MouseDownGeometry.GetLocalSize().Y)
	{
		bMouseDownInRegion = true;
	}

	return FReply::Unhandled();
}

FReply FActActionTimeSliderController::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();

	bool bHandleLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ActActionTimeSliderWidget->HasMouseCapture();
	bool bHandleRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ActActionTimeSliderWidget->HasMouseCapture() && TimeSliderArgs.AllowZoom;

	ActActionSequence::FActActionScrubRangeToScreen RangeToScreen = ActActionSequence::FActActionScrubRangeToScreen(TimeSliderArgs.ViewRange.Get(), MyGeometry.Size);
	FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

	if (bHandleRightMouseButton)
	{
		if (!bPanning)
		{
			// return unhandled in case our parent wants to use our right mouse button to open a context menu
			if (DistanceDragged == 0.f)
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
			FFrameTime MouseDownStart = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen);

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
				if (!TimeSliderArgs.ViewRange.IsBound())
				{
					// The output is not bound to a delegate so we'll manage the value ourselves
					TimeSliderArgs.ViewRange.Set(ViewRange);
				}
			}
		}
		else if (bMouseDownInRegion)
		{
			TimeSliderArgs.OnEndScrubberMovement.ExecuteIfBound();

			FFrameTime ScrubTime = MouseTime;
			FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();

			if (MouseDragType == ActActionSequence::EDragType::DRAG_SCRUBBING_TIME)
			{
				ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, CursorPos, RangeToScreen);
			}

			CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/false);
		}

		MouseDragType = ActActionSequence::EDragType::DRAG_NONE;
		DistanceDragged = 0.f;
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
				const bool bLockedPlayRange = TimeSliderArgs.IsPlaybackRangeLocked.Get();
				const float MouseDownPixel = RangeToScreen.InputToLocalX(MouseDownFree / TickResolution);
				ActActionSequence::FActActionScrubberMetrics ScrubberMetrics = ActActionSequence::ActActionStaticUtil::GetScrubPixelMetrics(TimeSliderArgs.DisplayRate.Get(), FQualifiedFrameTime(TimeSliderArgs.ScrubPosition.Get(), TickResolution), RangeToScreen, 4.0f);
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

	if (DistanceDragged != 0.f && (bHandleLeftMouseButton || bHandleRightMouseButton))
	{
		return FReply::Handled().CaptureMouse(ActActionTimeSliderWidget.ToSharedRef());
	}


	return FReply::Handled();
}

FReply FActActionTimeSliderController::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActActionSequence::FActActionTimeSliderArgs& TimeSliderArgs = GetTimeSliderArgs();
	TOptional<TRange<float>> NewTargetRange;

	if (TimeSliderArgs.AllowZoom && MouseEvent.IsControlDown())
	{
		float MouseFractionX = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()).X / MyGeometry.GetLocalSize().X;

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
