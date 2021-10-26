#include "ActEventTimelineSlider.h"

#include "ActEventTimelineImage.h"
#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "Common/NovaStaticFunction.h"
#include "Common/NovaStruct.h"
#include "NovaAct/NovaActEditor.h"

#include "NovaAct/ActEventTimeline/Image/Subs/ActActionTimeRangeSlider.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "LevelEditorViewport.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/ActEventTimeline/Image/ActActionSequenceSectionOverlayWidget.h"
#include "NovaAct/ActEventTimeline/Image/ActActionTimeSliderWidget.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActActionTimeRange.h"
#include "Widgets/Layout/SGridPanel.h"

FActEventTimelineSlider::FActEventTimelineSlider()
	: DistanceDragged(0),
	  MouseDragType(ENovaDragType::DRAG_NONE),
	  bMouseDownInRegion(false),
	  bPanning(false),
	  bMirrorLabels(false) {}

FActEventTimelineSlider::~FActEventTimelineSlider()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActEventTimelineSlider::~FActEventTimelineSlider"));
}

void FActEventTimelineSlider::Init(const TSharedRef<SGridPanel>& InParentGridPanel)
{
	// Create the top and bottom sliders
	ActActionTimeSliderWidget = SNew(SActActionTimeSliderWidget, SharedThis(this));
	TSharedRef<SActActionTimeRangeSlider> ActActionTimeRangeSlider = SNew(SActActionTimeRangeSlider, SharedThis(this));
	ActActionTimeRange = SNew(SActActionTimeRange, SharedThis(this), ActActionTimeRangeSlider);
	TickLinesSequenceSectionOverlayController = MakeShareable(new FActEventTimelineImage(SharedThis(this)));
	TickLinesSequenceSectionOverlayController->MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType::TickLines);
	ScrubPosSequenceSectionOverlayController = MakeShareable(new FActEventTimelineImage(SharedThis(this)));
	ScrubPosSequenceSectionOverlayController->MakeSequenceSectionOverlayWidget(ENovaSectionOverlayWidgetType::ScrubPosition);

	InParentGridPanel->AddSlot(1, 0, SGridPanel::Layer(10))
	                 .Padding(NovaConst::ResizeBarPadding)
	[
		SNew(SBorder)
	.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
	.BorderBackgroundColor(FLinearColor(.50f, .50f, .50f, 1.0f))
	.Padding(0)
	.Clipping(EWidgetClipping::ClipToBounds)
		[
			ActActionTimeSliderWidget.ToSharedRef()
		]
	];

	// ** 第1列，第1行，Overlay that draws the tick lines
	InParentGridPanel->AddSlot(1, 1, SGridPanel::Layer(10))
	                 .Padding(NovaConst::ResizeBarPadding)
	[
		ScrubPosSequenceSectionOverlayController->GetActActionSequenceSectionOverlayWidget()
	];

	// ** 第1列，第1行，Overlay that draws the scrub position
	InParentGridPanel->AddSlot(1, 1, SGridPanel::Layer(20))
	                 .Padding(NovaConst::ResizeBarPadding)
	[
		TickLinesSequenceSectionOverlayController->GetActActionSequenceSectionOverlayWidget()
	];

	// play range slider
	InParentGridPanel->AddSlot(1, 2, SGridPanel::Layer(10))
	                 .Padding(NovaConst::ResizeBarPadding)
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f, 1.0f))
		.Clipping(EWidgetClipping::ClipToBounds)
		.Padding(0)
		[
			ActActionTimeRange.ToSharedRef()
		]
	];
}

FFrameTime FActEventTimelineSlider::ComputeFrameTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, FActActionScrubRangeToScreen RangeToScreen, bool CheckSnapping) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	const double MouseValue = RangeToScreen.LocalXToInput(CursorPos.X);
	return MouseValue * ActEventTimelineArgs->TickResolution;
}

FFrameTime FActEventTimelineSlider::ComputeScrubTimeFromMouse(const FGeometry& Geometry, FVector2D ScreenSpacePosition, FActActionScrubRangeToScreen RangeToScreen) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const FVector2D CursorPos = Geometry.AbsoluteToLocal(ScreenSpacePosition);
	const double MouseSeconds = RangeToScreen.LocalXToInput(CursorPos.X);
	const FFrameTime ScrubTime = MouseSeconds * ActEventTimelineArgs->TickResolution;
	return ScrubTime;
}

void FActEventTimelineSlider::CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// TSharedPtr<FActEventTimelineEvents> ActEventTimelineEvents = GetActEventTimelineEvents();
	// The user can scrub past the viewing range of the time slider controller, so we clamp it to the view range.
	const TRange<float> ViewRange = ActEventTimelineArgs->ViewRange;
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	const FFrameTime LowerBound = (ViewRange.GetLowerBoundValue() * TickResolution).CeilToFrame();
	const FFrameTime UpperBound = (ViewRange.GetUpperBoundValue() * TickResolution).FloorToFrame();
	NewValue = FMath::Clamp(NewValue, LowerBound, UpperBound);
	// Manage the scrub position ourselves if its not bound to a delegate
	// if (!ActEventTimelineArgs->CurrentTime.IsBound())
	// {
	// ActEventTimelineArgs->CurrentTime.Set(NewValue);
	// }
	OnScrubPositionChanged(NewValue, bIsScrubbing);
}


void FActEventTimelineSlider::SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus)
{
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	DB->GetData()->PlaybackStatus = InPlaybackStatus;
	ESequencerState State = ESS_None;
	switch (InPlaybackStatus)
	{
	case ENovaPlaybackType::Playing:
	case ENovaPlaybackType::Recording:
		{
			State = ESS_Playing;
			break;
		}
	case ENovaPlaybackType::Stopped:
	case ENovaPlaybackType::Scrubbing:
	case ENovaPlaybackType::Stepping:
		{
			State = ESS_Paused;
			break;
		}
	default: ;
	}
	for (FLevelEditorViewportClient* LevelVC : GEditor->GetLevelViewportClients())
	{
		if (LevelVC && LevelVC->AllowsCinematicControl())
		{
			LevelVC->ViewState.GetReference()->SetSequencerState(State);
		}
	}
}

void FActEventTimelineSlider::OnBeginScrubberMovement()
{
	// Pause first since there's no explicit evaluation in the stopped state when OnEndScrubberMovement() is called
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
	SetPlaybackStatus(ENovaPlaybackType::Scrubbing);
}

void FActEventTimelineSlider::OnEndScrubberMovement()
{
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
}

void FActEventTimelineSlider::OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing)
{
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
	if (ActEventTimelineArgs->PlaybackStatus == ENovaPlaybackType::Scrubbing && !bScrubbing)
	{
		OnEndScrubberMovement();
	}
	const TWeakPtr<SWidget> PreviousFocusedWidget = FSlateApplication::Get().GetKeyboardFocusedWidget();
	// Clear focus before setting time in case there's a key editor value selected that gets committed to a newly selected key on UserMovedFocus
	if (ActEventTimelineArgs->PlaybackStatus == ENovaPlaybackType::Stopped)
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	}
	if (ActEventTimelineArgs->CurrentTime && NewScrubPosition != *ActEventTimelineArgs->CurrentTime)
	{
		*ActEventTimelineArgs->CurrentTime = NewScrubPosition;
		NovaDB::Trigger("ActEventTimelineArgs/CurrentTime");
	}
	if (PreviousFocusedWidget.IsValid())
	{
		FSlateApplication::Get().SetKeyboardFocus(PreviousFocusedWidget.Pin());
	}
}

void FActEventTimelineSlider::SetViewRange(double NewRangeMin, double NewRangeMax, ENovaViewRangeInterpolation Interpolation) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// Clamp to a minimum size to avoid zero-sized or negative visible ranges
	const double MinVisibleTimeRange = FFrameNumber(1) / ActEventTimelineArgs->TickResolution;
	const TRange<float> ExistingViewRange = ActEventTimelineArgs->ViewRange;

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
	// const TRange<float> NewRange = TRange<float>(NewRangeMin, NewRangeMax);
	UE_LOG(LogNovaAct, Log, TEXT("NewRangeMin : %f, NewRangeMax : %f"), NewRangeMin, NewRangeMax);

	// ActEventTimelineArgs->OnViewRangeChanged.ExecuteIfBound(NewRange, Interpolation);
}

bool FActEventTimelineSlider::HitTestRangeStart(const FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeStartPixel = RangeToScreen.InputToLocalX(Range.GetLowerBoundValue());

	// Hit test against the brush region to the right of the playback start position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeStartPixel - 4.0f && HitPixel <= RangeStartPixel + 10.0f;
}

bool FActEventTimelineSlider::HitTestRangeEnd(const FActActionScrubRangeToScreen& RangeToScreen, const TRange<double>& Range, float HitPixel) const
{
	const float RangeEndPixel = RangeToScreen.InputToLocalX(Range.GetUpperBoundValue());
	// Hit test against the brush region to the left of the playback end position, +/- DragToleranceSlateUnits
	return HitPixel >= RangeEndPixel - 10.0f && HitPixel <= RangeEndPixel + 4.0f;
}

// void FActEventTimelineSlider::SetPlaybackRangeStart(FFrameNumber NewStart) const
// {
// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
// 	const TRange<FFrameNumber> PlaybackRange = ActEventTimelineArgs->PlaybackRange;
//
// 	if (NewStart <= NovaStaticFunction::DiscreteExclusiveUpper(PlaybackRange.GetUpperBound()))
// 	{
// 		ActEventTimelineArgs->OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, PlaybackRange.GetUpperBound()));
// 	}
// }

// void FActEventTimelineSlider::SetPlaybackRangeEnd(FFrameNumber NewEnd) const
// {
// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
// 	const TRange<FFrameNumber> PlaybackRange = ActEventTimelineArgs->PlaybackRange;
//
// 	if (NewEnd >= NovaStaticFunction::DiscreteInclusiveLower(PlaybackRange.GetLowerBound()))
// 	{
// 		ActEventTimelineArgs->OnPlaybackRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(PlaybackRange.GetLowerBound(), TRangeBound<FFrameNumber>::Exclusive(NewEnd)));
// 	}
// }

// void FActEventTimelineSlider::SetSelectionRangeStart(FFrameNumber NewStart) const
// {
// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
// 	const TRange<FFrameNumber> SelectionRange = ActEventTimelineArgs->SelectionRange;
//
// 	if (SelectionRange.IsEmpty())
// 	{
// 		ActEventTimelineArgs->OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, NewStart + 1));
// 	}
// 	else if (NewStart <= NovaStaticFunction::DiscreteExclusiveUpper(SelectionRange.GetUpperBound()))
// 	{
// 		ActEventTimelineArgs->OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewStart, SelectionRange.GetUpperBound()));
// 	}
// }
//
// void FActEventTimelineSlider::SetSelectionRangeEnd(FFrameNumber NewEnd) const
// {
// 	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
// 	const TRange<FFrameNumber> SelectionRange = ActEventTimelineArgs->SelectionRange;
//
// 	if (SelectionRange.IsEmpty())
// 	{
// 		ActEventTimelineArgs->OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(NewEnd - 1, NewEnd));
// 	}
// 	else if (NewEnd >= NovaStaticFunction::DiscreteInclusiveLower(SelectionRange.GetLowerBound()))
// 	{
// 		ActEventTimelineArgs->OnSelectionRangeChanged.ExecuteIfBound(TRange<FFrameNumber>(SelectionRange.GetLowerBound(), NewEnd));
// 	}
// }

bool FActEventTimelineSlider::ZoomByDelta(float InDelta, float ZoomBias) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const TRange<float> LocalViewRange = ActEventTimelineArgs->ViewRange;
	const double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	const double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const double OutputViewSize = LocalViewRangeMax - LocalViewRangeMin;
	const double OutputChange = OutputViewSize * InDelta;

	const double NewViewOutputMin = LocalViewRangeMin - (OutputChange * ZoomBias);
	const double NewViewOutputMax = LocalViewRangeMax + (OutputChange * (1.f - ZoomBias));

	if (NewViewOutputMin < NewViewOutputMax)
	{
		SetViewRange(NewViewOutputMin, NewViewOutputMax, ENovaViewRangeInterpolation::Animated);
		return true;
	}

	return false;
}

void FActEventTimelineSlider::PanByDelta(float InDelta) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const TRange<float> LocalViewRange = ActEventTimelineArgs->ViewRange;

	const double CurrentMin = LocalViewRange.GetLowerBoundValue();
	const double CurrentMax = LocalViewRange.GetUpperBoundValue();

	// Adjust the delta to be a percentage of the current range
	InDelta *= 0.1f * (CurrentMax - CurrentMin);

	const double NewViewOutputMin = CurrentMin + InDelta;
	const double NewViewOutputMax = CurrentMax + InDelta;

	SetViewRange(NewViewOutputMin, NewViewOutputMax, ENovaViewRangeInterpolation::Animated);
}

FReply FActEventTimelineSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	MouseDragType = ENovaDragType::DRAG_NONE;
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

FReply FActEventTimelineSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// TSharedPtr<FActEventTimelineEvents> ActEventTimelineEvents = GetActEventTimelineEvents();
	const bool bHandleLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ActActionTimeSliderWidget->HasMouseCapture();
	const bool bHandleRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && ActActionTimeSliderWidget->HasMouseCapture() && ActEventTimelineArgs->AllowZoom;
	const FActActionScrubRangeToScreen RangeToScreen = FActActionScrubRangeToScreen(ActEventTimelineArgs->ViewRange, MyGeometry.Size);
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
		// if (MouseDragType == ENovaDragType::DRAG_PLAYBACK_START)
		// {
		// 	ActEventTimelineEvents->OnPlaybackRangeEndDrag.ExecuteIfBound();
		// }
		// else if (MouseDragType == ENovaDragType::DRAG_PLAYBACK_END)
		// {
		// 	ActEventTimelineEvents->OnPlaybackRangeEndDrag.ExecuteIfBound();
		// }
		// else if (MouseDragType == ENovaDragType::DRAG_SELECTION_START)
		// {
		// 	ActEventTimelineEvents->OnSelectionRangeEndDrag.ExecuteIfBound();
		// }
		// else if (MouseDragType == ENovaDragType::DRAG_SELECTION_END)
		// {
		// 	ActEventTimelineEvents->OnSelectionRangeEndDrag.ExecuteIfBound();
		// }
		// else if (MouseDragType == ENovaDragType::DRAG_MARK)
		// {
		// 	ActEventTimelineEvents->OnMarkEndDrag.ExecuteIfBound();
		// }
		if (MouseDragType == ENovaDragType::DRAG_SETTING_RANGE)
		{
			// Zooming
			const FFrameTime MouseDownStart = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen);

			const bool bCanZoomIn = MouseTime > MouseDownStart;
			const bool bCanZoomOut = ViewRangeStack.Num() > 0;
			if (bCanZoomIn || bCanZoomOut)
			{
				TRange<float> ViewRange = ActEventTimelineArgs->ViewRange;
				if (!bCanZoomIn)
				{
					ViewRange = ViewRangeStack.Pop();
				}

				if (bCanZoomIn)
				{
					// push the current value onto the stack
					ViewRangeStack.Add(ViewRange);

					ViewRange = TRange<float>(MouseDownStart.FrameNumber / ActEventTimelineArgs->TickResolution, MouseTime.FrameNumber / ActEventTimelineArgs->TickResolution);
				}

				// ActEventTimelineArgs->OnViewRangeChanged.ExecuteIfBound(ViewRange, ENovaViewRangeInterpolation::Immediate);
			}
		}
		else if (bMouseDownInRegion)
		{
			OnEndScrubberMovement();

			FFrameTime ScrubTime = MouseTime;
			const FVector2D CursorPos = MouseEvent.GetScreenSpacePosition();

			if (MouseDragType == ENovaDragType::DRAG_SCRUBBING_TIME)
			{
				ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, CursorPos, RangeToScreen);
			}

			CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/false);
		}

		MouseDragType = ENovaDragType::DRAG_NONE;
		DistanceDragged = 0.0f;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}

	bMouseDownInRegion = false;
	return FReply::Unhandled();
}

FReply FActEventTimelineSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// TSharedPtr<FActEventTimelineEvents> ActEventTimelineEvents = GetActEventTimelineEvents();
	bool bHandleLeftMouseButton = MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bHandleRightMouseButton = MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && ActEventTimelineArgs->AllowZoom;

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
			TRange<float> LocalViewRange = ActEventTimelineArgs->ViewRange;
			double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
			double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();

			FActActionScrubRangeToScreen ScaleInfo(LocalViewRange, MyGeometry.Size);
			FVector2D ScreenDelta = MouseEvent.GetCursorDelta();
			FVector2D InputDelta;
			InputDelta.X = ScreenDelta.X / ScaleInfo.PixelsPerInput;

			double NewViewOutputMin = LocalViewRangeMin - InputDelta.X;
			double NewViewOutputMax = LocalViewRangeMax - InputDelta.X;

			SetViewRange(NewViewOutputMin, NewViewOutputMax, ENovaViewRangeInterpolation::Immediate);
		}
	}
	else if (bHandleLeftMouseButton)
	{
		TRange<float> LocalViewRange = ActEventTimelineArgs->ViewRange;
		FActActionScrubRangeToScreen RangeToScreen(LocalViewRange, MyGeometry.Size);
		DistanceDragged += FMath::Abs(MouseEvent.GetCursorDelta().X);

		if (MouseDragType == ENovaDragType::DRAG_NONE)
		{
			if (DistanceDragged > FSlateApplication::Get().GetDragTriggerDistance())
			{
				// FFrameTime MouseDownFree = ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen, false);

				// const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
				// const FFrameRate DisplayRate = ActEventTimelineArgs->Tick;
				// const FQualifiedFrameTime QualifiedFrameTime = FQualifiedFrameTime(ActEventTimelineArgs->CurrentTime, TickResolution);
				// const bool bLockedPlayRange = ActEventTimelineArgs->IsPlaybackRangeLocked;
				// const float MouseDownPixel = RangeToScreen.InputToLocalX(MouseDownFree / TickResolution);
				// FActActionScrubberMetrics ScrubberMetrics = NovaStaticFunction::GetScrubPixelMetrics(TickResolution, QualifiedFrameTime, RangeToScreen, 4.0f);
				// const bool bHitScrubber = ScrubberMetrics.HandleRangePx.Contains(MouseDownPixel);

				// TRange<double> SelectionRange = ActEventTimelineArgs->SelectionRange / TickResolution;
				// TRange<double> PlaybackRange = ActEventTimelineArgs->PlaybackRange / TickResolution;

				// Disable selection range test if it's empty so that the playback range scrubbing gets priority
				// if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeEnd(RangeToScreen, SelectionRange, MouseDownPixel))
				// {
				// 	// selection range end scrubber
				// 	MouseDragType = ENovaDragType::DRAG_SELECTION_END;
				// 	ActEventTimelineArgs->OnSelectionRangeBeginDrag.ExecuteIfBound();
				// }
				// else if (!SelectionRange.IsEmpty() && !bHitScrubber && HitTestRangeStart(RangeToScreen, SelectionRange, MouseDownPixel))
				// {
				// 	// selection range start scrubber
				// 	MouseDragType = ENovaDragType::DRAG_SELECTION_START;
				// 	ActEventTimelineArgs->OnSelectionRangeBeginDrag.ExecuteIfBound();
				// }
				// else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeEnd(RangeToScreen, PlaybackRange, MouseDownPixel))
				// {
				// 	// playback range end scrubber
				// 	MouseDragType = ENovaDragType::DRAG_PLAYBACK_END;
				// 	ActEventTimelineArgs->OnPlaybackRangeBeginDrag.ExecuteIfBound();
				// }
				// else if (!bLockedPlayRange && !bHitScrubber && HitTestRangeStart(RangeToScreen, PlaybackRange, MouseDownPixel))
				// {
				// 	// playback range start scrubber
				// 	MouseDragType = ENovaDragType::DRAG_PLAYBACK_START;
				// 	ActEventTimelineArgs->OnPlaybackRangeBeginDrag.ExecuteIfBound();
				// }
				if (FSlateApplication::Get().GetModifierKeys().AreModifersDown(EModifierKey::Control))
				{
					MouseDragType = ENovaDragType::DRAG_SETTING_RANGE;
				}
				else if (bMouseDownInRegion)
				{
					MouseDragType = ENovaDragType::DRAG_SCRUBBING_TIME;
					auto DB = GetDataBinding(ENovaTransportControls, "TransportControlsState");
					DB->SetData(ENovaTransportControls::Pause);
					OnBeginScrubberMovement();
				}
			}
		}
		else
		{
			// FFrameTime MouseTime = ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);
			FFrameTime ScrubTime = ComputeScrubTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen);

			// Set the start range time?
			if (MouseDragType == ENovaDragType::DRAG_PLAYBACK_START)
			{
				// SetPlaybackRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ENovaDragType::DRAG_PLAYBACK_END)
			{
				// SetPlaybackRangeEnd(MouseTime.FrameNumber);
			}
			else if (MouseDragType == ENovaDragType::DRAG_SELECTION_START)
			{
				// SetSelectionRangeStart(MouseTime.FrameNumber);
			}
				// Set the end range time?
			else if (MouseDragType == ENovaDragType::DRAG_SELECTION_END)
			{
				// SetSelectionRangeEnd(MouseTime.FrameNumber);
			}
			else if (MouseDragType == ENovaDragType::DRAG_SCRUBBING_TIME)
			{
				// Delegate responsibility for clamping to the current view range to the client
				CommitScrubPosition(ScrubTime, /*bIsScrubbing=*/true);
			}
			else if (MouseDragType == ENovaDragType::DRAG_SETTING_RANGE)
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

FReply FActEventTimelineSlider::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();


	TOptional<TRange<float>> NewTargetRange;

	if (ActEventTimelineArgs->AllowZoom && MouseEvent.IsControlDown())
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

//
// ActActionSequence::FActEventTimelineArgs& FActEventTimelineSlider::GetActEventTimelineArgs() const
// {
// 	return ActActionSequenceController.Pin()->GetActEventTimelineArgs();
// }

void FActEventTimelineSlider::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActActionScrubRangeToScreen& RangeToScreen, const FActActionDrawTickArgs& InArgs) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	// const FFrameRate DisplayRate = ActEventTimelineArgs->DisplayRate;
	const FPaintGeometry PaintGeometry = InArgs.AllottedGeometry.ToPaintGeometry();
	const FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 8);

	double MajorGridStep = 0.0;
	int32 MinorDivisions = 0;
	if (!NovaStaticFunction::GetGridMetrics(ActEventTimelineArgs->NumericTypeInterface, InArgs.AllottedGeometry.Size.X, ViewRange.GetLowerBoundValue(), ViewRange.GetUpperBoundValue(), MajorGridStep, MinorDivisions))
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
	const FFrameNumber FrameNumber = ConvertFrameTime(*ActEventTimelineArgs->CurrentTime, TickResolution, TickResolution).FloorToFrame();
	const float FlooredScrubPx = RangeToScreen.InputToLocalX(FrameNumber / TickResolution);
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
			FString FrameString = ActEventTimelineArgs->NumericTypeInterface->ToString((CurrentMajorLine * TickResolution).RoundToFrame().Value);

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

TSharedRef<FActEventTimelineArgs> FActEventTimelineSlider::GetActEventTimelineArgs() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	return ActEventTimelineArgs.ToSharedRef();
}

// TSharedRef<FActEventTimelineEvents> FActEventTimelineSlider::GetActEventTimelineEvents() const
// {
// 	auto ActEventTimelineEventsDB = GetDataBindingSP(FActEventTimelineEvents, "ActEventTimelineEvents");
// 	TSharedPtr<FActEventTimelineEvents> ActEventTimelineEvents = ActEventTimelineEventsDB->GetData();
// 	return ActEventTimelineEvents.ToSharedRef();
// }
