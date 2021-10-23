#include "ActActionSequenceNotifyNode.h"

#include "SCurveEditor.h"
#include "Common/NovaEnum.h"
#include "Common/NovaConst.h"
#include "Fonts/FontMeasure.h"
#include "NovaAct/ActEventTimeline/Operation/ActTrackAreaSlotDragDrop.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionTrackAreaSlot.h"

void SActActionSequenceNotifyNode::Construct(const FArguments& InArgs, const TSharedRef<FActActionTrackAreaSlot>& InTrackAreaSlot)
{
	ActActionTrackAreaSlot = InTrackAreaSlot;
	// Font = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	bBeingDragged = false;
	CurrentDragHandle = ENovaNotifyStateHandleHit::None;
	bDrawTooltipToRight = true;
	bSelected = false;

	// OnNodeDragStarted = InArgs._OnNodeDragStarted;
	OnNotifyStateHandleBeingDragged = InArgs._OnNotifyStateHandleBeingDragged;
	PanTrackRequest = InArgs._PanTrackRequest;
	OnSelectionChanged = InArgs._OnSelectionChanged;
	OnUpdatePanel = InArgs._OnUpdatePanel;
	OnSnapPosition = InArgs._OnSnapPosition;

	SetClipping(EWidgetClipping::ClipToBounds);
	SetToolTipText(TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateLambda([this]()
	{
		return ActActionTrackAreaSlot.Pin()->GetNodeTooltip();
	})));
}

int32 SActActionSequenceNotifyNode::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const FActActionTrackAreaArgs& TrackAreaArgs = ActActionTrackAreaSlot.Pin()->GetActActionTrackAreaArgs();
	int32 MarkerLayer = LayerId + 1;
	int32 ScrubHandleID = MarkerLayer + 1;
	int32 TextLayerID = ScrubHandleID + 1;
	int32 BranchPointLayerID = TextLayerID + 1;
	const FSlateBrush* StyleInfo = FEditorStyle::GetBrush(TEXT("SpecialEditableTextImageNormal"));
	FText Text = GetNotifyText();
	FLinearColor NodeColor = GetNotifyColor();
	FLinearColor BoxColor = bSelected ? FEditorStyle::GetSlateColor("SelectionColor").GetSpecifiedColor() : GetNotifyColor();
	float TickResolutionInterval = (float)TrackAreaArgs.TickResolution.Get().AsInterval();
	float HalfScrubHandleWidth = ScrubHandleSize.X / 2.0f;
	// Show duration of AnimNotifyState
	if (NotifyDurationSizeX > 0.f)
	{
		FVector2D DurationBoxSize = FVector2D(NotifyDurationSizeX, TextSize.Y + TextBorderSize.Y * 2.f);
		FVector2D DurationBoxPosition = FVector2D(NotifyScrubHandleCentre, (NovaConst::NotifyHeight - TextSize.Y) * 0.5f);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(DurationBoxPosition, DurationBoxSize),
			StyleInfo,
			ESlateDrawEffect::None,
			BoxColor);

		DrawScrubHandle(DurationBoxPosition.X + DurationBoxSize.X, OutDrawElements, ScrubHandleID, AllottedGeometry, MyCullingRect, NodeColor);

		float EndTime = TrackAreaArgs.End.Get() * (float)TickResolutionInterval;
		if (EndTime != ActActionTrackAreaSlot.Pin()->GetPlayLength())//Don't render offset when we are at the end of the sequence, doesnt help the user
		{
			// ScrubHandle
			float HandleCentre = NotifyDurationSizeX + (ScrubHandleSize.X - 2.0f);
			DrawHandleOffset(0.5f, HandleCentre, OutDrawElements, MarkerLayer, AllottedGeometry, MyCullingRect, NodeColor);
		}
	}

	// Branching point
	// bool bDrawBranchingPoint = AnimNotifyEvent && AnimNotifyEvent->IsBranchingPoint();
	bool bDrawBranchingPoint = false;

	// Background
	FVector2D LabelSize = TextSize + TextBorderSize * 2.f;
	// LabelSize.X += HalfScrubHandleWidth + (bDrawBranchingPoint ? (BranchingPointIconSize.X + TextBorderSize.X * 2.f) : 0.f);
	LabelSize.X += HalfScrubHandleWidth;

	FVector2D LabelPosition(bDrawTooltipToRight ? NotifyScrubHandleCentre : NotifyScrubHandleCentre - LabelSize.X, (NovaConst::NotifyHeight - TextSize.Y) * 0.5f);

	if (NotifyDurationSizeX == 0.f)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(LabelPosition, LabelSize),
			StyleInfo,
			ESlateDrawEffect::None,
			BoxColor);
	}

	// Text
	FVector2D TextPosition = LabelPosition + TextBorderSize;
	if (bDrawTooltipToRight)
	{
		TextPosition.X += HalfScrubHandleWidth;
	}

	FVector2D DrawTextSize;
	DrawTextSize.X = (NotifyDurationSizeX > 0.0f ? FMath::Min(NotifyDurationSizeX - (ScrubHandleSize.X + (bDrawBranchingPoint ? BranchingPointIconSize.X : 0)), TextSize.X) : TextSize.X);
	DrawTextSize.Y = TextSize.Y;

	if (bDrawBranchingPoint)
	{
		TextPosition.X += BranchingPointIconSize.X;
	}

	FPaintGeometry TextGeometry = AllottedGeometry.ToPaintGeometry(TextPosition, DrawTextSize);
	OutDrawElements.PushClip(FSlateClippingZone(TextGeometry));

	FSlateDrawElement::MakeText(
		OutDrawElements,
		TextLayerID,
		TextGeometry,
		Text,
		Font,
		ESlateDrawEffect::None,
		FLinearColor::Black
	);

	OutDrawElements.PopClip();

	// Draw Branching Point
	if (bDrawBranchingPoint)
	{
		FVector2D BranchPointIconPos = LabelPosition + TextBorderSize;
		if (bDrawTooltipToRight)
		{
			BranchPointIconPos.X += HalfScrubHandleWidth;
		}
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			BranchPointLayerID,
			AllottedGeometry.ToPaintGeometry(BranchPointIconPos, BranchingPointIconSize),
			FEditorStyle::GetBrush(TEXT("AnimNotifyEditor.BranchingPoint")),
			ESlateDrawEffect::None,
			FLinearColor::White
		);
	}

	DrawScrubHandle(NotifyScrubHandleCentre, OutDrawElements, ScrubHandleID, AllottedGeometry, MyCullingRect, NodeColor);

	float BeginTime = TrackAreaArgs.Begin.Get() * (float)TickResolutionInterval;
	if (BeginTime > 0.f && BeginTime != ActActionTrackAreaSlot.Pin()->GetPlayLength())//Don't render offset when we are at the start/end of the sequence, doesn't help the user
	{
		DrawHandleOffset(0.5f, NotifyScrubHandleCentre, OutDrawElements, MarkerLayer, AllottedGeometry, MyCullingRect, NodeColor);
	}

	return TextLayerID;
}

FReply SActActionSequenceNotifyNode::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	FVector2D ScreenNodePosition = MyGeometry.AbsolutePosition;

	// Whether the drag has hit a duration marker
	bool bDragOnMarker = false;
	bBeingDragged = true;

	if (NotifyDurationSizeX > 0.0f)
	{
		// This is a state node, check for a drag on the markers before movement. Use last screen space position before the drag started
		// as using the last position in the mouse event gives us a mouse position after the drag was started.
		ENovaNotifyStateHandleHit MarkerHit = DurationHandleHitTest(LastMouseDownPosition);
		if (MarkerHit == ENovaNotifyStateHandleHit::Start || MarkerHit == ENovaNotifyStateHandleHit::End)
		{
			bDragOnMarker = true;
			bBeingDragged = false;
			CurrentDragHandle = MarkerHit;
		}
	}

	return OnNotifyNodeDragStarted(MouseEvent, ScreenNodePosition, bDragOnMarker);
}


FCursorReply SActActionSequenceNotifyNode::OnCursorQuery(const FGeometry& MyGeometry, const FPointerEvent& CursorEvent) const
{
	// Show resize cursor if the cursor is hovering over either of the scrub handles of a notify state node
	if (IsHovered() && NotifyDurationSizeX > 0.0f)
	{
		FVector2D RelMouseLocation = MyGeometry.AbsoluteToLocal(CursorEvent.GetScreenSpacePosition());

		const float HandleHalfWidth = ScrubHandleSize.X / 2.0f;
		const float DistFromFirstHandle = FMath::Abs(RelMouseLocation.X - NotifyScrubHandleCentre);
		const float DistFromSecondHandle = FMath::Abs(RelMouseLocation.X - (NotifyScrubHandleCentre + NotifyDurationSizeX));

		if (DistFromFirstHandle < HandleHalfWidth || DistFromSecondHandle < HandleHalfWidth || CurrentDragHandle != ENovaNotifyStateHandleHit::None)
		{
			return FCursorReply::Cursor(EMouseCursor::ResizeLeftRight);
		}
	}
	return FCursorReply::Unhandled();
}

FReply SActActionSequenceNotifyNode::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	// Don't do scrub handle dragging if we haven't captured the mouse.
	if (!this->HasMouseCapture())
	{
		return FReply::Unhandled();
	}

	if (CurrentDragHandle == ENovaNotifyStateHandleHit::None)
	{
		// We've had focus taken away - release the mouse
		FSlateApplication::Get().ReleaseAllPointerCapture();
		return FReply::Unhandled();
	}
	const FActActionTrackAreaArgs& TrackAreaArgs = ActActionTrackAreaSlot.Pin()->GetActActionTrackAreaArgs();
	FTrackScaleInfo ScaleInfo(TrackAreaArgs.ViewInputMin.Get(), TrackAreaArgs.ViewInputMax.Get(), 0, 0, CachedAllottedGeometrySize);
	float PlayLength = ActActionTrackAreaSlot.Pin()->GetPlayLength();
	float XPositionInTrack = MyGeometry.AbsolutePosition.X - CachedTrackGeometry.AbsolutePosition.X;
	float TrackScreenSpaceXPosition = MyGeometry.AbsolutePosition.X - XPositionInTrack;
	// float TrackScreenSpaceOrigin = CachedTrackGeometry.LocalToAbsolute(FVector2D(ScaleInfo.InputToLocalX(0.0f), 0.0f)).X;
	// float TrackScreenSpaceLimit = CachedTrackGeometry.LocalToAbsolute(FVector2D(ScaleInfo.InputToLocalX(PlayLength), 0.0f)).X;
	float TickResolutionInterval = (float)TrackAreaArgs.TickResolution.Get().AsInterval();
	TSharedRef<FActActionSequenceTreeViewNode> ActActionSequenceTreeViewNode = ActActionTrackAreaSlot.Pin()->GetActActionSequenceTreeViewNode();
	if (CurrentDragHandle == ENovaNotifyStateHandleHit::Start)
	{
		// Check track bounds
		float OldDisplayTime = TrackAreaArgs.Begin.Get() * TickResolutionInterval;
		float DurationTime = (TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * TickResolutionInterval;
		if (MouseEvent.GetScreenSpacePosition().X >= TrackScreenSpaceXPosition && MouseEvent.GetScreenSpacePosition().X <= TrackScreenSpaceXPosition + CachedAllottedGeometrySize.X)
		{
			float NewDisplayTime = ScaleInfo.LocalXToInput((MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition + XPositionInTrack).X);
			float NewDuration = OldDisplayTime - NewDisplayTime;
			// Check to make sure the duration is not less than the minimum allowed
			if (NewDuration < NovaConst::ActMinimumNotifyStateFrame)
			{
				NewDisplayTime -= NovaConst::ActMinimumNotifyStateFrame - NewDuration;
			}
			NewDisplayTime = FMath::Max(0.0f, NewDisplayTime);
			ActActionSequenceTreeViewNode->SetHitBoxBegin((int)(NewDisplayTime / TickResolutionInterval));
			float NewDurationTime = DurationTime + OldDisplayTime - NewDisplayTime;
			ActActionSequenceTreeViewNode->SetHitBoxDuration((int)(NewDurationTime / TickResolutionInterval));
		}
		else if (DurationTime > NovaConst::ActMinimumNotifyStateFrame)
		{
			// float Overflow = HandleOverflowPan(MouseEvent.GetScreenSpacePosition(), TrackScreenSpaceXPosition, TrackScreenSpaceOrigin, TrackScreenSpaceLimit);
			// Update scale info to the new view inputs after panning
			ScaleInfo.ViewMinInput = TrackAreaArgs.ViewInputMin.Get();
			ScaleInfo.ViewMaxInput = TrackAreaArgs.ViewInputMax.Get();

			float NewDisplayTime = FMath::Max(0.0f, ScaleInfo.LocalXToInput((MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition + XPositionInTrack).X));
			ActActionSequenceTreeViewNode->SetHitBoxBegin((int)(NewDisplayTime / TickResolutionInterval));
			float NewDurationTime = DurationTime + OldDisplayTime - NewDisplayTime;
			ActActionSequenceTreeViewNode->SetHitBoxDuration((int)(NewDurationTime / TickResolutionInterval));

			// Adjust in case we went under the minimum
			// if ((int)(NewDurationTime / TickResolutionInterval) < NovaConst::ActMinimumNotifyStateFrame)
			// {
			// 	float EndTimeBefore = NodeObjectInterface->GetTime() + NodeObjectInterface->GetDuration();
			// 	NodeObjectInterface->SetTime(NodeObjectInterface->GetTime() + NodeObjectInterface->GetDuration() - NovaConst::ActMinimumNotifyStateFrame);
			// 	NodeObjectInterface->SetDuration(NovaConst::ActMinimumNotifyStateFrame);
			// 	float EndTimeAfter = NodeObjectInterface->GetTime() + NodeObjectInterface->GetDuration();
			// }
		}

		// // Now we know where the marker should be, look for possible snaps on montage marker bars
		// if (FAnimNotifyEvent* AnimNotifyEvent = NodeObjectInterface->GetNotifyEvent())
		// {
		// 	float InputStartTime = AnimNotifyEvent->GetTime();
		// 	float MarkerSnap = GetScrubHandleSnapPosition(InputStartTime, ENovaNotifyStateHandleHit::Start);
		// 	if (MarkerSnap != -1.0f)
		// 	{
		// 		// We're near to a snap bar
		// 		EAnimEventTriggerOffsets::Type Offset = (MarkerSnap < InputStartTime) ? EAnimEventTriggerOffsets::OffsetAfter : EAnimEventTriggerOffsets::OffsetBefore;
		// 		AnimNotifyEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);
		//
		// 		// Adjust our start marker
		// 		OldDisplayTime = AnimNotifyEvent->GetTime();
		// 		AnimNotifyEvent->SetTime(MarkerSnap);
		// 		AnimNotifyEvent->SetDuration(AnimNotifyEvent->GetDuration() + OldDisplayTime - AnimNotifyEvent->GetTime());
		// 	}
		// 	else
		// 	{
		// 		AnimNotifyEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
		// 	}
		// }
		OnNotifyStateHandleBeingDragged.ExecuteIfBound(SharedThis(this), MouseEvent, CurrentDragHandle, TrackAreaArgs.Begin.Get() * (float)TrackAreaArgs.TickResolution.Get().AsInterval());
	}
	else
	{
		float BeginTime = TrackAreaArgs.Begin.Get() * TickResolutionInterval;
		float DurationTime = (TrackAreaArgs.End.Get() - TrackAreaArgs.Begin.Get()) * TickResolutionInterval;
		if (MouseEvent.GetScreenSpacePosition().X >= TrackScreenSpaceXPosition && MouseEvent.GetScreenSpacePosition().X <= TrackScreenSpaceXPosition + CachedAllottedGeometrySize.X)
		{
			float NewDurationTime = ScaleInfo.LocalXToInput((MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition + XPositionInTrack).X) - BeginTime;
			NewDurationTime = FMath::Max(NewDurationTime, (float)(NovaConst::ActMinimumNotifyStateFrame * TickResolutionInterval));
			ActActionSequenceTreeViewNode->SetHitBoxDuration((int)(NewDurationTime / TickResolutionInterval));
		}
		else if ((int)(DurationTime / TickResolutionInterval) > NovaConst::ActMinimumNotifyStateFrame)
		{
			// float Overflow = HandleOverflowPan(MouseEvent.GetScreenSpacePosition(), TrackScreenSpaceXPosition, TrackScreenSpaceOrigin, TrackScreenSpaceLimit);
			// Update scale info to the new view inputs after panning
			ScaleInfo.ViewMinInput = TrackAreaArgs.ViewInputMin.Get();
			ScaleInfo.ViewMaxInput = TrackAreaArgs.ViewInputMax.Get();

			float NewDurationTime = ScaleInfo.LocalXToInput((MouseEvent.GetScreenSpacePosition() - MyGeometry.AbsolutePosition + XPositionInTrack).X) - BeginTime;
			NewDurationTime = FMath::Max(NewDurationTime, (float)(NovaConst::ActMinimumNotifyStateFrame * TickResolutionInterval));
			ActActionSequenceTreeViewNode->SetHitBoxDuration((int)(NewDurationTime / TickResolutionInterval));
		}
		if (BeginTime + DurationTime > PlayLength)
		{
			ActActionSequenceTreeViewNode->SetHitBoxDuration((int)((PlayLength - BeginTime) / TickResolutionInterval));
		}

		// // Now we know where the scrub handle should be, look for possible snaps on montage marker bars
		// if (FAnimNotifyEvent* AnimNotifyEvent = NodeObjectInterface->GetNotifyEvent())
		// {
		// 	float InputEndTime = AnimNotifyEvent->GetTime() + AnimNotifyEvent->GetDuration();
		// 	float MarkerSnap = GetScrubHandleSnapPosition(InputEndTime, ENovaNotifyStateHandleHit::End);
		// 	if (MarkerSnap != -1.0f)
		// 	{
		// 		// We're near to a snap bar
		// 		EAnimEventTriggerOffsets::Type Offset = (MarkerSnap < InputEndTime) ? EAnimEventTriggerOffsets::OffsetAfter : EAnimEventTriggerOffsets::OffsetBefore;
		// 		AnimNotifyEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);
		//
		// 		// Adjust our end marker
		// 		AnimNotifyEvent->SetDuration(MarkerSnap - AnimNotifyEvent->GetTime());
		// 	}
		// 	else
		// 	{
		// 		AnimNotifyEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
		// 	}
		// }
		OnNotifyStateHandleBeingDragged.ExecuteIfBound(SharedThis(this), MouseEvent, CurrentDragHandle, BeginTime + DurationTime);
	}

	return FReply::Handled();
}

FReply SActActionSequenceNotifyNode::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bool bLeftButton = MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;

	if (bLeftButton && CurrentDragHandle != ENovaNotifyStateHandleHit::None)
	{
		// Clear the drag marker and give the mouse back
		CurrentDragHandle = ENovaNotifyStateHandleHit::None;

		// Signal selection changing so details panels get updated
		OnSelectionChanged.ExecuteIfBound();

		// End drag transaction before handing mouse back
		// check(DragMarkerTransactionIdx != INDEX_NONE);
		// GEditor->EndTransaction();
		// DragMarkerTransactionIdx = INDEX_NONE;

		// Sequence->PostEditChange();
		// Sequence->MarkPackageDirty();

		OnUpdatePanel.ExecuteIfBound();

		return FReply::Handled().ReleaseMouseCapture();
	}

	return FReply::Unhandled();
}


FVector2D SActActionSequenceNotifyNode::ComputeDesiredSize(float) const
{
	return WidgetSize;
}

void SActActionSequenceNotifyNode::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
	CachedTrackGeometry = AllottedGeometry;
	ScreenPosition = AllottedGeometry.AbsolutePosition;
}

FText SActActionSequenceNotifyNode::GetNotifyText() const
{
	// Combine comment from notify struct and from function on object
	return FText::FromName(ActActionTrackAreaSlot.Pin()->GetName());
}

FLinearColor SActActionSequenceNotifyNode::GetNotifyColor() const
{
	TOptional<FLinearColor> Color = ActActionTrackAreaSlot.Pin()->GetEditorColor();
	FLinearColor BaseColor = Color.Get(FLinearColor(1, 1, 0.5f));
	BaseColor.A = 0.67f;
	return BaseColor;
}


void SActActionSequenceNotifyNode::UpdateSizeAndPosition(const FGeometry& AllottedGeometry)
{
	FActActionTrackAreaArgs& ActActionTrackAreaArgs = ActActionTrackAreaSlot.Pin()->GetActActionTrackAreaArgs();
	FTrackScaleInfo ScaleInfo(ActActionTrackAreaArgs.ViewInputMin.Get(), ActActionTrackAreaArgs.ViewInputMax.Get(), 0, 0, AllottedGeometry.Size);
	float TickResolutionInterval = (float)ActActionTrackAreaArgs.TickResolution.Get().AsInterval();
	// Cache the geometry information, the allotted geometry is the same size as the track.
	CachedAllottedGeometrySize = AllottedGeometry.Size * AllottedGeometry.Scale;

	float BeginTime = ActActionTrackAreaArgs.Begin.Get() * TickResolutionInterval;
	float DurationTime = (ActActionTrackAreaArgs.End.Get() - ActActionTrackAreaArgs.Begin.Get()) * TickResolutionInterval;
	NotifyTimePositionX = ScaleInfo.InputToLocalX(BeginTime);
	NotifyDurationSizeX = ScaleInfo.PixelsPerInput * DurationTime;

	const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();
	TextSize = FontMeasureService->Measure(GetNotifyText(), Font);
	LabelWidth = TextSize.X + (TextBorderSize.X * 2.f) + (ScrubHandleSize.X / 2.f);

	bool bDrawBranchingPoint = ActActionTrackAreaSlot.Pin()->IsBranchingPoint();
	BranchingPointIconSize = FVector2D(TextSize.Y, TextSize.Y);
	if (bDrawBranchingPoint)
	{
		LabelWidth += BranchingPointIconSize.X + TextBorderSize.X * 2.f;
	}

	//Calculate scrub handle box size (the notional box around the scrub handle and the alignment marker)
	float NotifyHandleBoxWidth = FMath::Max(ScrubHandleSize.X, AlignmentMarkerSize.X * 2);

	// Work out where we will have to draw the tool tip
	float LeftEdgeToNotify = NotifyTimePositionX;
	float RightEdgeToNotify = AllottedGeometry.Size.X - NotifyTimePositionX;
	bDrawTooltipToRight = NotifyDurationSizeX > 0.0f || ((RightEdgeToNotify > LabelWidth) || (RightEdgeToNotify > LeftEdgeToNotify));

	// Calculate widget width/position based on where we are drawing the tool tip
	WidgetX = bDrawTooltipToRight ? (NotifyTimePositionX - (NotifyHandleBoxWidth / 2.f)) : (NotifyTimePositionX - LabelWidth);
	WidgetSize = bDrawTooltipToRight ? FVector2D((NotifyDurationSizeX > 0.0f ? NotifyDurationSizeX : FMath::Max(LabelWidth, NotifyDurationSizeX)), NovaConst::NotifyHeight) : FVector2D((LabelWidth + NotifyDurationSizeX), NovaConst::NotifyHeight);
	WidgetSize.X += NotifyHandleBoxWidth;
	// Widget position of the notify marker
	NotifyScrubHandleCentre = bDrawTooltipToRight ? NotifyHandleBoxWidth / 2.f : LabelWidth;
}

void SActActionSequenceNotifyNode::DrawScrubHandle(float ScrubHandleCentre, FSlateWindowElementList& OutDrawElements, int32 ScrubHandleID, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FLinearColor NodeColour) const
{
	FVector2D ScrubHandlePosition(ScrubHandleCentre - ScrubHandleSize.X / 2.0f, (NovaConst::NotifyHeight - ScrubHandleSize.Y) / 2.f);
	FSlateDrawElement::MakeBox(
		OutDrawElements,
		ScrubHandleID,
		AllottedGeometry.ToPaintGeometry(ScrubHandlePosition, ScrubHandleSize),
		FEditorStyle::GetBrush(TEXT("Sequencer.KeyDiamond")),
		ESlateDrawEffect::None,
		NodeColour
	);

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		ScrubHandleID,
		AllottedGeometry.ToPaintGeometry(ScrubHandlePosition, ScrubHandleSize),
		FEditorStyle::GetBrush(TEXT("Sequencer.KeyDiamondBorder")),
		ESlateDrawEffect::None,
		bSelected ? FEditorStyle::GetSlateColor("SelectionColor").GetSpecifiedColor() : FLinearColor::Black
	);
}


void SActActionSequenceNotifyNode::DrawHandleOffset(const float& Offset, const float& HandleCentre, FSlateWindowElementList& OutDrawElements, int32 MarkerLayer, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FLinearColor NodeColor) const
{
	FVector2D MarkerPosition;
	FVector2D MarkerSize = AlignmentMarkerSize;

	if (Offset < 0.f)
	{
		MarkerPosition.Set(HandleCentre - AlignmentMarkerSize.X, (NovaConst::NotifyHeight - AlignmentMarkerSize.Y) / 2.f);
	}
	else
	{
		MarkerPosition.Set(HandleCentre + AlignmentMarkerSize.X, (NovaConst::NotifyHeight - AlignmentMarkerSize.Y) / 2.f);
		MarkerSize.X = -AlignmentMarkerSize.X;
	}

	FSlateDrawElement::MakeBox(
		OutDrawElements,
		MarkerLayer,
		AllottedGeometry.ToPaintGeometry(MarkerPosition, MarkerSize),
		FEditorStyle::GetBrush(TEXT("Sequencer.Timeline.NotifyAlignmentMarker")),
		ESlateDrawEffect::None,
		NodeColor
	);
}


ENovaNotifyStateHandleHit SActActionSequenceNotifyNode::DurationHandleHitTest(const FVector2D& CursorTrackPosition) const
{
	ENovaNotifyStateHandleHit MarkerHit = ENovaNotifyStateHandleHit::None;

	// Make sure this node has a duration box (meaning it is a state node)
	if (NotifyDurationSizeX > 0.0f)
	{
		// Test for mouse inside duration box with handles included
		float ScrubHandleHalfWidth = ScrubHandleSize.X / 2.0f;

		// Position and size of the notify node including the scrub handles
		FVector2D NotifyNodePosition(NotifyScrubHandleCentre - ScrubHandleHalfWidth, 0.0f);
		FVector2D NotifyNodeSize(NotifyDurationSizeX + ScrubHandleHalfWidth * 2.0f, NovaConst::NotifyHeight);

		FVector2D MouseRelativePosition(CursorTrackPosition - FVector2D(WidgetX, NovaConst::NotifyHeightOffset));

		if (MouseRelativePosition > NotifyNodePosition && MouseRelativePosition < (NotifyNodePosition + NotifyNodeSize))
		{
			// Definitely inside the duration box, need to see which handle we hit if any
			if (MouseRelativePosition.X <= (NotifyNodePosition.X + ScrubHandleSize.X))
			{
				// Left Handle
				MarkerHit = ENovaNotifyStateHandleHit::Start;
			}
			else if (MouseRelativePosition.X >= (NotifyNodePosition.X + NotifyNodeSize.X - ScrubHandleSize.X))
			{
				// Right Handle
				MarkerHit = ENovaNotifyStateHandleHit::End;
			}
		}
	}

	return MarkerHit;
}

/** @return the Node's position within the track */
FVector2D SActActionSequenceNotifyNode::GetWidgetPosition() const
{
	return FVector2D(WidgetX, NovaConst::NotifyHeightOffset);
}


float SActActionSequenceNotifyNode::HandleOverflowPan(const FVector2D& ScreenCursorPos, float TrackScreenSpaceXPosition, float TrackScreenSpaceMin, float TrackScreenSpaceMax)
{
	float Overflow = 0.0f;

	if (ScreenCursorPos.X < TrackScreenSpaceXPosition && TrackScreenSpaceXPosition > TrackScreenSpaceMin - 10.0f)
	{
		// Overflow left edge
		Overflow = FMath::Min(ScreenCursorPos.X - TrackScreenSpaceXPosition, -10.0f);
	}
	else if (ScreenCursorPos.X > CachedAllottedGeometrySize.X && (TrackScreenSpaceXPosition + CachedAllottedGeometrySize.X) < TrackScreenSpaceMax + 10.0f)
	{
		// Overflow right edge
		Overflow = FMath::Max(ScreenCursorPos.X - (TrackScreenSpaceXPosition + CachedAllottedGeometrySize.X), 10.0f);
	}

	PanTrackRequest.ExecuteIfBound(Overflow, CachedAllottedGeometrySize);

	return Overflow;
}


FReply SActActionSequenceNotifyNode::OnNotifyNodeDragStarted(const FPointerEvent& MouseEvent, const FVector2D& InScreenNodePosition, const bool bDragOnMarker)
{
	// Check to see if we've already selected the triggering node
	// if (!NotifyNode->bSelected)
	// {
	// 	SelectTrackObjectNode(NotifyIndex, MouseEvent.IsShiftDown(), false);
	// }

	// Sort our nodes so we're acessing them in time order
	// SelectedNodeIndices.Sort([this](const int32& A, const int32& B)
	// {
	// 	float TimeA = NotifyNodes[A]->NodeObjectInterface->GetTime();
	// 	float TimeB = NotifyNodes[B]->NodeObjectInterface->GetTime();
	// 	return TimeA < TimeB;
	// });

	// If we're dragging one of the direction markers we don't need to call any further as we don't want the drag drop op
	if (!bDragOnMarker)
	{
		// TArray<TSharedPtr<SActActionSequenceNotifyNode>> NodesToDrag;
		// const float FirstNodeX = GetWidgetPosition().X;
		//
		// for (auto Iter = SelectedNodeIndices.CreateIterator(); Iter; ++Iter)
		// {
		// 	TSharedPtr<SActActionSequenceNotifyNode> Node = NotifyNodes[*Iter];
		// 	NodesToDrag.Add(Node);
		// }

		// TSharedRef<SOverlay> Decorator = SNew(SOverlay);
		// FVector2D ScreenNodePosition = CachedTrackGeometry.LocalToAbsolute(GetWidgetPosition());
		const FVector2D ScreenCursorPos = MouseEvent.GetScreenSpacePosition();
		TSharedRef<SOverlay> NodeDragDecoratorOverlay = SNew(SOverlay);
		TSharedRef<SBorder> NodeDragDecorator = SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				NodeDragDecoratorOverlay
			];

		// TArray<TSharedPtr<SAnimNotifyNode>> Nodes;
		//
		// for (TSharedPtr<SAnimNotifyTrack> Track : NotifyAnimTracks)
		// {
		// 	Track->DisconnectSelectedNodesForDrag(Nodes);
		// }

		FBox2D OverlayBounds(ScreenPosition, ScreenPosition + FVector2D(NotifyDurationSizeX, 0.0f));
		// for (int32 Idx = 1; Idx < Nodes.Num(); ++Idx)
		// {
		// 	TSharedPtr<SAnimNotifyNode> Node = Nodes[Idx];
		// 	FVector2D NodePosition = Node->GetScreenPosition();
		// 	float NodeDuration = Node->GetDurationSize();
		//
		// 	OverlayBounds += FBox2D(NodePosition, NodePosition + FVector2D(NodeDuration, 0.0f));
		// }

		FVector2D OverlayOrigin = OverlayBounds.Min;
		FVector2D OverlayExtents = OverlayBounds.GetSize();

		// for (TSharedPtr<SAnimNotifyNode> Node : Nodes)
		// {
		FVector2D OffsetFromFirst(ScreenPosition - OverlayOrigin);

		NodeDragDecoratorOverlay->AddSlot()
		                        .Padding(FMargin(OffsetFromFirst.X, OffsetFromFirst.Y, 0.0f, 0.0f))
		[
			AsShared()
		];
		// }

		// FPanTrackRequest PanRequestDelegate = FPanTrackRequest::CreateSP(this, &SAnimNotifyPanel::PanInputViewRange);
		// FOnUpdatePanel UpdateDelegate = FOnUpdatePanel::CreateSP(this, &SAnimNotifyPanel::Update);
		return FReply::Handled().BeginDragDrop(FActTrackAreaSlotDragDrop::New(SharedThis(this), NodeDragDecorator, ScreenCursorPos, OverlayOrigin, OverlayExtents, CurrentDragXPosition));
	}
	else
	{
		// Capture the mouse in the node
		return FReply::Handled().CaptureMouse(AsShared()).UseHighPrecisionMouseMovement(AsShared());
	}
}

FVector2D SActActionSequenceNotifyNode::GetNotifyPositionOffset() const
{
	return GetNotifyPosition() - GetWidgetPosition();
}

FVector2D SActActionSequenceNotifyNode::GetNotifyPosition() const
{
	return FVector2D(NotifyTimePositionX, NovaConst::NotifyHeightOffset);
}
