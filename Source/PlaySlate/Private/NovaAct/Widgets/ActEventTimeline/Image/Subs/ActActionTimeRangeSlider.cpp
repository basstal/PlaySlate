#include "ActActionTimeRangeSlider.h"

#include "PlaySlate.h"
#include "NovaAct/ActEventTimeline/ActEventTimelineSlider.h"


SActActionTimeRangeSlider::SActActionTimeRangeSlider()
	: bHandleDragged(false),
	  bLeftHandleDragged(false),
	  bRightHandleDragged(false),
	  bHandleHovered(false),
	  bLeftHandleHovered(false),
	  bRightHandleHovered(false)
{}

SActActionTimeRangeSlider::~SActActionTimeRangeSlider()
{
	UE_LOG(LogActAction, Log, TEXT("SActActionTimeRangeSlider::~SActActionTimeRangeSlider"));
}

void SActActionTimeRangeSlider::Construct(const FArguments& InArgs, const TSharedRef<FActEventTimelineSlider>& InTimeSliderController)
{
	TimeSliderController = InTimeSliderController;
	// LastViewRange = InTimeSliderController->GetTimeSliderArgs().ViewRange.Get();

	ResetState();
}

FVector2D SActActionTimeRangeSlider::ComputeDesiredSize(float LayoutScaleMultiplier) const
{
	return FVector2D(56.0f, HandleSize);
}

int32 SActActionTimeRangeSlider::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	static const FSlateBrush* RangeHandleLeft = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.RangeHandleLeft"));
	static const FSlateBrush* RangeHandleRight = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.RangeHandleRight"));
	static const FSlateBrush* RangeHandle = FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.RangeHandle"));

	float LeftHandleOffset = 0.f;
	float HandleOffset = 0.f;
	float RightHandleOffset = 0.f;
	ComputeHandleOffsets(LeftHandleOffset, RightHandleOffset, HandleOffset, AllottedGeometry.GetLocalSize().X);

	static const FName SelectionColorName("SelectionColor");
	const FLinearColor SelectionColor = FEditorStyle::GetSlateColor(SelectionColorName).GetColor(FWidgetStyle());

	// Draw the handle box
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(HandleOffset, 0.0f), FVector2D(RightHandleOffset - LeftHandleOffset - HandleSize, HandleSize)),
		RangeHandle,
		ESlateDrawEffect::None,
		(bHandleDragged || bHandleHovered) ? SelectionColor : FLinearColor::Gray);

	// Draw the left handle box
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(LeftHandleOffset, 0.0f), FVector2D(HandleSize, HandleSize)),
		RangeHandleLeft,
		ESlateDrawEffect::None,
		(bLeftHandleDragged || bLeftHandleHovered) ? SelectionColor : FLinearColor::Gray);

	// Draw the right handle box
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		LayerId,
		AllottedGeometry.ToPaintGeometry(FVector2D(RightHandleOffset, 0.0f), FVector2D(HandleSize, HandleSize)),
		RangeHandleRight,
		ESlateDrawEffect::None,
		(bRightHandleDragged || bRightHandleHovered) ? SelectionColor : FLinearColor::Gray);

	SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, ShouldBeEnabled(bParentEnabled));

	return LayerId;
}

FReply SActActionTimeRangeSlider::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	MouseDownPosition = MouseEvent.GetScreenSpacePosition();
	MouseDownViewRange = TRange<float>(ActEventTimelineArgs->ViewRange);

	if (bHandleHovered)
	{
		bHandleDragged = true;
		return FReply::Handled().CaptureMouse(AsShared());
	}
	else if (bLeftHandleHovered)
	{
		bLeftHandleDragged = true;
		return FReply::Handled().CaptureMouse(AsShared());
	}
	else if (bRightHandleHovered)
	{
		bRightHandleDragged = true;
		return FReply::Handled().CaptureMouse(AsShared());
	}

	return FReply::Unhandled();
}

FReply SActActionTimeRangeSlider::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ResetState();

	return FReply::Handled().ReleaseMouseCapture();
}

FReply SActActionTimeRangeSlider::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (HasMouseCapture())
	{
		const float DragDelta = ComputeDragDelta(MouseEvent, MyGeometry.GetLocalSize().X);

		if (!TimeSliderController.IsValid())
		{
			return FReply::Handled();
		}

		if (bHandleDragged)
		{
			auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
			double NewIn = MouseDownViewRange.GetLowerBoundValue() + DragDelta;
			double NewOut = MouseDownViewRange.GetUpperBoundValue() + DragDelta;
			const TRange<float> ClampRange = ActEventTimelineArgs->ClampRange;
			if (NewIn < ClampRange.GetLowerBoundValue())
			{
				NewIn = ClampRange.GetLowerBoundValue();
				NewOut = NewIn + (MouseDownViewRange.GetUpperBoundValue() - MouseDownViewRange.GetLowerBoundValue());
			}
			else if (NewOut > ClampRange.GetUpperBoundValue())
			{
				NewOut = ClampRange.GetUpperBoundValue();
				NewIn = NewOut - (MouseDownViewRange.GetUpperBoundValue() - MouseDownViewRange.GetLowerBoundValue());
			}
			TimeSliderController.Pin()->SetViewRange(NewIn, NewOut, ENovaViewRangeInterpolation::Immediate);
		}
		else if (bLeftHandleDragged || bRightHandleDragged)
		{
			double NewIn, NewOut;
			if (bLeftHandleDragged)
			{
				NewIn = MouseDownViewRange.GetLowerBoundValue() + DragDelta;

				NewOut = MouseDownViewRange.GetUpperBoundValue();
				if (MouseEvent.IsShiftDown())
				{
					NewOut -= DragDelta;
				}
			}
			else
			{
				NewIn = MouseDownViewRange.GetLowerBoundValue();
				if (MouseEvent.IsShiftDown())
				{
					NewIn -= DragDelta;
				}

				NewOut = MouseDownViewRange.GetUpperBoundValue() + DragDelta;
			}

			// In cases of extreme zoom the drag delta will be greater than the difference between In/Out.
			// This causes zooming to then become pan at extreme levels which is undesirable.
			if (NewIn >= NewOut)
			{
				return FReply::Handled();
			}

			TimeSliderController.Pin()->SetViewRange(NewIn, NewOut, ENovaViewRangeInterpolation::Immediate);
		}

		return FReply::Handled();
	}
	else
	{
		ResetHoveredState();

		float LeftHandleOffset = 0.f;
		float HandleOffset = 0.f;
		float RightHandleOffset = 0.f;
		ComputeHandleOffsets(LeftHandleOffset, RightHandleOffset, HandleOffset, MyGeometry.GetLocalSize().X);

		const FGeometry LeftHandleRect = MyGeometry.MakeChild(FVector2D(LeftHandleOffset, 0.f), FVector2D(HandleSize, HandleSize));
		const FGeometry RightHandleRect = MyGeometry.MakeChild(FVector2D(RightHandleOffset, 0.f), FVector2D(HandleSize, HandleSize));
		const FGeometry HandleRect = MyGeometry.MakeChild(FVector2D(HandleOffset, 0.0f), FVector2D(RightHandleOffset - LeftHandleOffset - HandleSize, HandleSize));
		const FVector2D LocalMousePosition = MouseEvent.GetScreenSpacePosition();

		if (HandleRect.IsUnderLocation(LocalMousePosition))
		{
			bHandleHovered = true;
		}
		else if (LeftHandleRect.IsUnderLocation(LocalMousePosition))
		{
			bLeftHandleHovered = true;
		}
		else if (RightHandleRect.IsUnderLocation(LocalMousePosition))
		{
			bRightHandleHovered = true;
		}
	}
	return FReply::Unhandled();
}

void SActActionTimeRangeSlider::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	SCompoundWidget::OnMouseLeave(MouseEvent);

	if (!HasMouseCapture())
	{
		ResetHoveredState();
	}
}

FReply SActActionTimeRangeSlider::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	ResetState();

	OnMouseMove(InMyGeometry, InMouseEvent);

	// if (bHandleHovered && TimeSliderController.IsValid())
	// {
	// 	if (!LastViewRange.IsEmpty())
	// 	{
	// 		TimeSliderController.Pin()->SetViewRange(LastViewRange.GetLowerBoundValue(), LastViewRange.GetUpperBoundValue(), ENovaViewRangeInterpolation::Immediate);
	// 	}
	// }
	ResetState();
	return FReply::Unhandled();
}

void SActActionTimeRangeSlider::ComputeHandleOffsets(float& LeftHandleOffset, float& RightHandleOffset, float& HandleOffset, int32 GeometryWidth) const
{
	auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	float InTime = ActEventTimelineArgs->ViewRange.GetLowerBoundValue();
	float OutTime = ActEventTimelineArgs->ViewRange.GetUpperBoundValue();
	float EndTime = ActEventTimelineArgs->ClampRange.GetUpperBoundValue();
	float BeginTime = ActEventTimelineArgs->ClampRange.GetLowerBoundValue();
	if (EndTime - BeginTime <= 0)
	{
		return;
	}
	const float UnitsToPixel = (GeometryWidth - HandleSize * 2) / (EndTime - BeginTime);

	LeftHandleOffset = (InTime - BeginTime) * UnitsToPixel;
	HandleOffset = LeftHandleOffset + HandleSize;
	RightHandleOffset = HandleOffset + (OutTime - InTime) * UnitsToPixel;

	const float ScrubberWidth = RightHandleOffset - LeftHandleOffset - HandleSize;
	if (ScrubberWidth < HandleSize * 2)
	{
		HandleOffset = HandleOffset - (HandleSize * 2 - ScrubberWidth) / 2.f;
		LeftHandleOffset = HandleOffset - HandleSize;
		RightHandleOffset = HandleOffset + HandleSize * 2;
	}
}

float SActActionTimeRangeSlider::ComputeDragDelta(const FPointerEvent& MouseEvent, int32 GeometryWidth) const
{
	auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
	float BeginTime = ActEventTimelineArgs->ClampRange.GetLowerBoundValue();
	float EndTime = ActEventTimelineArgs->ClampRange.GetUpperBoundValue();
	const float DragDistance = (MouseEvent.GetScreenSpacePosition() - MouseDownPosition).X;
	const float PixelToUnits = (EndTime - BeginTime) / (GeometryWidth - HandleSize * 2);
	const float DragDelta = DragDistance * PixelToUnits;
	UE_LOG(LogActAction, Log, TEXT("DragDelta : %f"), DragDelta);
	return DragDelta;
}

void SActActionTimeRangeSlider::ResetState()
{
	bHandleDragged = false;
	bLeftHandleDragged = false;
	bRightHandleDragged = false;
	ResetHoveredState();
}

void SActActionTimeRangeSlider::ResetHoveredState()
{
	bHandleHovered = false;
	bLeftHandleHovered = false;
	bRightHandleHovered = false;
}
