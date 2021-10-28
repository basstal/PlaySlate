#include "ActSliderWidget.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "Common/NovaStaticFunction.h"
#include "Common/NovaStruct.h"
#include "NovaAct/NovaActEditor.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "ITransportControl.h"
#include "NovaAct/ActEventTimeline/Image/ActImageScrubPosition.h"
#include "NovaAct/ActEventTimeline/Slider/ActSliderViewRangeWidget.h"

#include "LevelEditorViewport.h"
#include "Widgets/Layout/SGridPanel.h"

SActSliderWidget::SActSliderWidget()
	: DistanceDragged(0),
	  MouseDragType(ENovaDragType::DRAG_NONE),
	  bMouseDownInRegion(false),
	  bPanning(false),
	  bMirrorLabels(false) {}

SActSliderWidget::~SActSliderWidget()
{
	UE_LOG(LogNovaAct, Log, TEXT("SActSliderWidget::~SActSliderWidget"));
	NovaDB::Delete("ActEventTimelineArgs/ViewRange");
}

void SActSliderWidget::Construct(const FArguments& InArgs, const TSharedRef<SGridPanel>& InParentGridPanel)
{
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	NovaDB::CreateSP("ActEventTimelineArgs/ViewRange", DB->GetData()->ViewRange);
	FDelegateHandle _;
	DataBindingSPBindRaw(TRange<float>, "ActEventTimelineArgs/ViewRange", this, &SActSliderWidget::OnViewRangeChanged, _);

	InParentGridPanel->AddSlot(1, 0, SGridPanel::Layer(10))
	                 .Padding(NovaConst::ResizeBarPadding)
	[
		SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		.BorderBackgroundColor(FLinearColor(.50f, .50f, .50f, 1.0f))
		.Padding(0)
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			this->AsShared()
		]
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
			SNew(SActSliderViewRangeWidget)
		]
	];
}

int32 SActSliderWidget::OnPaint(const FPaintArgs& Args,
                                const FGeometry& AllottedGeometry,
                                const FSlateRect& MyCullingRect,
                                FSlateWindowElementList& OutDrawElements,
                                int32 LayerId,
                                const FWidgetStyle& InWidgetStyle,
                                bool bParentEnabled) const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	const TRange<float> ViewRange = *ActEventTimelineArgs->ViewRange;
	const float LocalSequenceLength = ViewRange.Size<float>();
	if (LocalSequenceLength <= 0)
	{
		return SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	}

	FActSliderScrubRangeToScreen RangeToScreen(ViewRange, AllottedGeometry.Size);

	// ** draw tick marks
	const float MajorTickHeight = 9.0f;
	FActDrawTickArgs DrawTickArgs;
	{
		DrawTickArgs.AllottedGeometry = AllottedGeometry;
		DrawTickArgs.bMirrorLabels = bMirrorLabels;
		DrawTickArgs.bOnlyDrawMajorTicks = false;
		DrawTickArgs.TickColor = FLinearColor::White;
		DrawTickArgs.CullingRect = MyCullingRect;
		DrawTickArgs.DrawEffects = bParentEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
		// Draw major ticks under sections
		DrawTickArgs.StartLayer = LayerId;
		// Draw the tick the entire height of the section area
		DrawTickArgs.TickOffset = bMirrorLabels ? 0.0f : FMath::Abs(AllottedGeometry.Size.Y - MajorTickHeight);
		DrawTickArgs.MajorTickHeight = MajorTickHeight;
	}
	DrawTicks(OutDrawElements, ViewRange, RangeToScreen, DrawTickArgs);

	// Draw the scrub handle
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	FQualifiedFrameTime ScrubPosition = FQualifiedFrameTime(FFrameTime(ActEventTimelineArgs->CurrentTime->GetFrame()), TickResolution);
	FActSliderScrubberMetrics ScrubberMetrics = NovaStaticFunction::MakePixelScrubberMetrics(TickResolution, ScrubPosition, RangeToScreen);
	const float HandleStart = ScrubberMetrics.HandleRangePx.GetLowerBoundValue();
	const float HandleEnd = ScrubberMetrics.HandleRangePx.GetUpperBoundValue();
	FPaintGeometry PaintGeometry = AllottedGeometry.ToPaintGeometry(FVector2D(HandleStart, 0),
	                                                                FVector2D(HandleEnd - HandleStart, AllottedGeometry.Size.Y));
	FLinearColor ScrubColor = InWidgetStyle.GetColorAndOpacityTint();
	// TODO:Sequencer this color should be specified in the style
	ScrubColor.A *= 0.75f;
	ScrubColor.B *= 0.1f;
	ScrubColor.G *= 0.2f;
	const FSlateBrush* FrameBlockScrubHandleUpBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.FrameBlockScrubHandleUp"));
	const FSlateBrush* FrameBlockScrubHandleDownBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.FrameBlockScrubHandleDown"));
	const FSlateBrush* VanillaScrubHandleUpBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.VanillaScrubHandleUp"));
	const FSlateBrush* VanillaScrubHandleDownBrush = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.VanillaScrubHandleDown"));
	const FSlateBrush* Brush =
		ScrubberMetrics.Style == EActSliderScrubberStyle::Vanilla ? (bMirrorLabels ? VanillaScrubHandleUpBrush : VanillaScrubHandleDownBrush) : (bMirrorLabels ? FrameBlockScrubHandleUpBrush : FrameBlockScrubHandleDownBrush);
	const int32 ArrowLayer = LayerId + 2;
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		ArrowLayer,
		PaintGeometry,
		Brush,
		DrawTickArgs.DrawEffects,
		ScrubColor
	);

	// Draw the current time next to the scrub handle
	FString FrameString = ActEventTimelineArgs->NumericTypeInterface->ToString(ActEventTimelineArgs->CurrentTime->GetFrame().Value);
	FSlateFontInfo SmallLayoutFont = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	const TSharedRef<FSlateFontMeasure> SlateFontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	FVector2D TextSize = SlateFontMeasureService->Measure(FrameString, SmallLayoutFont);

	// Flip the text position if getting near the end of the view range
	bool bDrawLeft = (AllottedGeometry.Size.X - HandleEnd) < (TextSize.X + 12.0f);
	float TextPosition = bDrawLeft ? (HandleStart - TextSize.X - 2.0f) : (HandleEnd + 2.0f);
	FVector2D TextOffset(TextPosition, bMirrorLabels ? (DrawTickArgs.AllottedGeometry.Size.Y - TextSize.Y) : 0.0f);
	FSlateDrawElement::MakeText(
		OutDrawElements,
		DrawTickArgs.StartLayer + 1,
		DrawTickArgs.AllottedGeometry.ToPaintGeometry(TextOffset, TextSize),
		FrameString,
		SmallLayoutFont,
		DrawTickArgs.DrawEffects,
		FLinearColor::Yellow
	);

	if (MouseDragType == ENovaDragType::DRAG_SETTING_RANGE)
	{
		// ** 按下 Ctrl 的情况下 在 时间轴 上拖拽选择一段时间，可以缩放至这段时间的功能
		FActSliderScrubRangeToScreen MouseDownRange(ViewRange, MouseDownGeometry.Size);
		FFrameTime DownPosition0Time = NovaStaticFunction::ComputeFrameTimeFromMouse(MouseDownGeometry, MouseDownPosition[0], MouseDownRange, TickResolution);
		FFrameTime DownPosition1Time = NovaStaticFunction::ComputeFrameTimeFromMouse(MouseDownGeometry, MouseDownPosition[1], MouseDownRange, TickResolution);
		float MouseStartPosX = RangeToScreen.InputToLocalX(DownPosition0Time / TickResolution);
		float MouseEndPosX = RangeToScreen.InputToLocalX(DownPosition1Time / TickResolution);
		float RangePosX = MouseStartPosX < MouseEndPosX ? MouseStartPosX : MouseEndPosX;
		float RangeSizeX = FMath::Abs(MouseStartPosX - MouseEndPosX);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(FVector2D(RangePosX, 0.0f), FVector2D(RangeSizeX, AllottedGeometry.Size.Y)),
			bMirrorLabels ? VanillaScrubHandleDownBrush : VanillaScrubHandleUpBrush,
			DrawTickArgs.DrawEffects,
			MouseStartPosX < MouseEndPosX ? FLinearColor(0.5f, 0.5f, 0.5f) : FLinearColor(0.25f, 0.3f, 0.3f)
		);
	}

	return ArrowLayer;
}


FReply SActSliderWidget::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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

FReply SActSliderWidget::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	const bool bHasMouseCapture = HasMouseCapture();
	const bool bHandleLeftMouseButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bHasMouseCapture;
	const bool bHandleRightMouseButton = MouseEvent.GetEffectingButton() == EKeys::RightMouseButton && bHasMouseCapture && ActEventTimelineArgs->AllowZoom;
	const FActSliderScrubRangeToScreen RangeToScreen = FActSliderScrubRangeToScreen(*ActEventTimelineArgs->ViewRange, MyGeometry.Size);
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	const FFrameTime MouseTime = NovaStaticFunction::ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen, TickResolution);

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
			const FFrameTime MouseDownStart = NovaStaticFunction::ComputeFrameTimeFromMouse(MyGeometry, MouseDownPosition[0], RangeToScreen, TickResolution);

			const bool bCanZoomIn = MouseTime > MouseDownStart;
			const bool bCanZoomOut = ViewRangeZoomStack.Num() > 0;
			if (bCanZoomIn || bCanZoomOut)
			{
				if (bCanZoomIn)
				{
					// push the current value onto the stack
					ViewRangeZoomStack.Add(*ActEventTimelineArgs->ViewRange);
					ActEventTimelineArgs->ViewRange->SetLowerBoundValue(MouseDownStart.FrameNumber / TickResolution);
					ActEventTimelineArgs->ViewRange->SetUpperBoundValue(MouseTime.FrameNumber / TickResolution);
				}
				else
				{
					TRange<float> ViewRangePopped = ViewRangeZoomStack.Pop();
					ActEventTimelineArgs->ViewRange->SetLowerBoundValue(ViewRangePopped.GetLowerBoundValue());
					ActEventTimelineArgs->ViewRange->SetUpperBoundValue(ViewRangePopped.GetUpperBoundValue());
				}
				NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
			}
		}
		else if (bMouseDownInRegion)
		{
			OnEndScrubberMovement();
			CommitScrubPosition(MouseTime, /*bIsScrubbing=*/false);
		}

		MouseDragType = ENovaDragType::DRAG_NONE;
		DistanceDragged = 0.0f;
		bMouseDownInRegion = false;

		return FReply::Handled().ReleaseMouseCapture();
	}

	bMouseDownInRegion = false;
	return FReply::Unhandled();
}

FReply SActSliderWidget::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	bool bHandleLeftMouseButton = MouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton);
	bool bHandleRightMouseButton = MouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && ActEventTimelineArgs->AllowZoom;
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;

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
			FActSliderScrubRangeToScreen RangeToScreen(*ActEventTimelineArgs->ViewRange, MyGeometry.Size);
			FVector2D ScreenDelta = MouseEvent.GetCursorDelta();
			FVector2D InputDelta(ScreenDelta.X / RangeToScreen.PixelsPerInput, 0);
			double NewViewOutputMin = ActEventTimelineArgs->ViewRange->GetLowerBoundValue() - InputDelta.X;
			double NewViewOutputMax = ActEventTimelineArgs->ViewRange->GetUpperBoundValue() - InputDelta.X;
			ActEventTimelineArgs->ViewRange->SetLowerBoundValue(NewViewOutputMin);
			ActEventTimelineArgs->ViewRange->SetUpperBoundValue(NewViewOutputMax);
			NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
		}
	}
	else if (bHandleLeftMouseButton)
	{
		FActSliderScrubRangeToScreen RangeToScreen(*ActEventTimelineArgs->ViewRange, MyGeometry.Size);
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
				// FActSliderScrubberMetrics ScrubberMetrics = NovaStaticFunction::MakePixelScrubberMetrics(TickResolution, QualifiedFrameTime, RangeToScreen, 4.0f);
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
					auto DB = GetDataBinding(EPlaybackMode::Type, "PreviewInstancePlaybackMode");
					DB->SetData(EPlaybackMode::Stopped);
					OnBeginScrubberMovement();
				}
			}
		}
		else
		{
			FFrameTime ScrubTime = NovaStaticFunction::ComputeFrameTimeFromMouse(MyGeometry, MouseEvent.GetScreenSpacePosition(), RangeToScreen, TickResolution);

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
		return FReply::Handled().CaptureMouse(AsShared());
	}


	return FReply::Handled();
}

FReply SActSliderWidget::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
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


void SActSliderWidget::CommitScrubPosition(FFrameTime NewValue, bool bIsScrubbing)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// The user can scrub past the viewing range of the time slider controller, so we clamp it to the view range.
	const TRange<float> ViewRange = *ActEventTimelineArgs->ViewRange;
	const FFrameRate TickResolution = ActEventTimelineArgs->TickResolution;
	const FFrameTime LowerBound = (ViewRange.GetLowerBoundValue() * TickResolution).CeilToFrame();
	const FFrameTime UpperBound = (ViewRange.GetUpperBoundValue() * TickResolution).FloorToFrame();
	NewValue = FMath::Clamp(NewValue, LowerBound, UpperBound);
	OnScrubPositionChanged(NewValue, bIsScrubbing);
}


void SActSliderWidget::SetPlaybackStatus(ENovaPlaybackType InPlaybackStatus)
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

void SActSliderWidget::OnBeginScrubberMovement()
{
	// Pause first since there's no explicit evaluation in the stopped state when OnEndScrubberMovement() is called
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
	SetPlaybackStatus(ENovaPlaybackType::Scrubbing);
}

void SActSliderWidget::OnEndScrubberMovement()
{
	SetPlaybackStatus(ENovaPlaybackType::Stopped);
}

void SActSliderWidget::OnScrubPositionChanged(FFrameTime NewScrubPosition, bool bScrubbing)
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

void SActSliderWidget::OnViewRangeChanged(TSharedPtr<TRange<float>> InViewRange)
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	// Clamp to a minimum size to avoid zero-sized or negative visible ranges
	const double MinVisibleTimeRange = FFrameNumber(1) / ActEventTimelineArgs->TickResolution;
	float NewRangeMax = InViewRange->GetUpperBoundValue();
	float NewRangeMin = InViewRange->GetLowerBoundValue();
	if (NewRangeMin > NewRangeMax - MinVisibleTimeRange)
	{
		NewRangeMin = NewRangeMax - MinVisibleTimeRange;
	}
	// Clamp to the clamp range
	ActEventTimelineArgs->ViewRange->SetLowerBoundValue(NewRangeMin);
	ActEventTimelineArgs->ViewRange->SetUpperBoundValue(NewRangeMax);
}

bool SActSliderWidget::ZoomByDelta(float InDelta, float ZoomBias) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	TRange<float> LocalViewRange = *ActEventTimelineArgs->ViewRange;
	const double LocalViewRangeMax = LocalViewRange.GetUpperBoundValue();
	const double LocalViewRangeMin = LocalViewRange.GetLowerBoundValue();
	const double OutputViewSize = LocalViewRangeMax - LocalViewRangeMin;
	const double OutputChange = OutputViewSize * InDelta;

	const double NewViewOutputMin = LocalViewRangeMin - (OutputChange * ZoomBias);
	const double NewViewOutputMax = LocalViewRangeMax + (OutputChange * (1.f - ZoomBias));

	if (NewViewOutputMin < NewViewOutputMax)
	{
		LocalViewRange.SetLowerBoundValue(NewViewOutputMin);
		LocalViewRange.SetUpperBoundValue(NewViewOutputMax);
		NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
		return true;
	}

	return false;
}

void SActSliderWidget::PanByDelta(float InDelta) const
{
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = GetActEventTimelineArgs();
	TRange<float> LocalViewRange = *ActEventTimelineArgs->ViewRange;

	const double CurrentMin = LocalViewRange.GetLowerBoundValue();
	const double CurrentMax = LocalViewRange.GetUpperBoundValue();

	// Adjust the delta to be a percentage of the current range
	InDelta *= 0.1f * (CurrentMax - CurrentMin);

	const double NewViewOutputMin = CurrentMin + InDelta;
	const double NewViewOutputMax = CurrentMax + InDelta;

	LocalViewRange.SetLowerBoundValue(NewViewOutputMin);
	LocalViewRange.SetUpperBoundValue(NewViewOutputMax);
	NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
}

void SActSliderWidget::DrawTicks(FSlateWindowElementList& OutDrawElements, const TRange<float>& ViewRange, const FActSliderScrubRangeToScreen& RangeToScreen, const FActDrawTickArgs& InArgs)
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
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

TSharedRef<FActEventTimelineArgs> SActSliderWidget::GetActEventTimelineArgs() const
{
	auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	return ActEventTimelineArgs.ToSharedRef();
}
