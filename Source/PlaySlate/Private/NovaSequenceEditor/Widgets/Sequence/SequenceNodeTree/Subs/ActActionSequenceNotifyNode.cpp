#include "ActActionSequenceNotifyNode.h"

#include "SCurveEditor.h"
#include "Fonts/FontMeasure.h"
#include "NovaSequenceEditor/Controllers/Sequence/SequenceNodeTree/ActActionTrackAreaSlot.h"
#include "Utils/ActActionBasics.h"

void SActActionSequenceNotifyNode::Construct(const FArguments& InArgs, const TSharedRef<FActActionTrackAreaSlot>& InTrackAreaSlot)
{
	ActActionTrackAreaSlot = InTrackAreaSlot;
	// Font = FCoreStyle::GetDefaultFontStyle("Regular", 10);
	bBeingDragged = false;
	CurrentDragHandle = ActActionSequence::ENotifyStateHandleHit::None;
	bDrawTooltipToRight = true;
	bSelected = false;

	OnNodeDragStarted = InArgs._OnNodeDragStarted;
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
	int32 MarkerLayer = LayerId + 1;
	int32 ScrubHandleID = MarkerLayer + 1;
	int32 TextLayerID = ScrubHandleID + 1;
	int32 BranchPointLayerID = TextLayerID + 1;

	// FAnimNotifyEvent* AnimNotifyEvent = ActActionTrackAreaSlot.Pin()->GetNotifyEvent();

	const FSlateBrush* StyleInfo = FEditorStyle::GetBrush(TEXT("SpecialEditableTextImageNormal"));

	FText Text = GetNotifyText();
	FLinearColor NodeColor = GetNotifyColor();
	FLinearColor BoxColor = bSelected ? FEditorStyle::GetSlateColor("SelectionColor").GetSpecifiedColor() : GetNotifyColor();

	float HalfScrubHandleWidth = ScrubHandleSize.X / 2.0f;
	float NotifyHeight = ActActionTrackAreaSlot.Pin()->NotifyHeight;
	// Show duration of AnimNotifyState
	if (NotifyDurationSizeX > 0.f)
	{
		FVector2D DurationBoxSize = FVector2D(NotifyDurationSizeX, TextSize.Y + TextBorderSize.Y * 2.f);
		FVector2D DurationBoxPosition = FVector2D(NotifyScrubHandleCentre, (NotifyHeight - TextSize.Y) * 0.5f);
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(DurationBoxPosition, DurationBoxSize),
			StyleInfo,
			ESlateDrawEffect::None,
			BoxColor);

		DrawScrubHandle(DurationBoxPosition.X + DurationBoxSize.X, OutDrawElements, ScrubHandleID, AllottedGeometry, MyCullingRect, NodeColor);

		// // Render offsets if necessary
		// if (AnimNotifyEvent && AnimNotifyEvent->EndTriggerTimeOffset != 0.f) //Do we have an offset to render?
		// {
		// 	float EndTime = AnimNotifyEvent->GetTime() + AnimNotifyEvent->GetDuration();
		// 	if (EndTime != ActActionTrackAreaSlot.Pin()->GetPlayLength()) //Don't render offset when we are at the end of the sequence, doesnt help the user
		// 	{
		// 		// ScrubHandle
		// 		float HandleCentre = NotifyDurationSizeX + (ScrubHandleSize.X - 2.0f);
		// 		DrawHandleOffset(AnimNotifyEvent->EndTriggerTimeOffset, HandleCentre, OutDrawElements, MarkerLayer, AllottedGeometry, MyCullingRect, NodeColor);
		// 	}
		// }
	}

	// Branching point
	// bool bDrawBranchingPoint = AnimNotifyEvent && AnimNotifyEvent->IsBranchingPoint();
	bool bDrawBranchingPoint = false;

	// Background
	FVector2D LabelSize = TextSize + TextBorderSize * 2.f;
	// LabelSize.X += HalfScrubHandleWidth + (bDrawBranchingPoint ? (BranchingPointIconSize.X + TextBorderSize.X * 2.f) : 0.f);
	LabelSize.X += HalfScrubHandleWidth;

	FVector2D LabelPosition(bDrawTooltipToRight ? NotifyScrubHandleCentre : NotifyScrubHandleCentre - LabelSize.X, (NotifyHeight - TextSize.Y) * 0.5f);

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

	// if (AnimNotifyEvent && AnimNotifyEvent->TriggerTimeOffset != 0.f) //Do we have an offset to render?
	// {
	// 	float NotifyTime = AnimNotifyEvent->GetTime();
	// 	if (NotifyTime != 0.f && NotifyTime != ActActionTrackAreaSlot.Pin()->GetPlayLength()) //Don't render offset when we are at the start/end of the sequence, doesn't help the user
	// 	{
	// 		DrawHandleOffset(AnimNotifyEvent->TriggerTimeOffset, NotifyScrubHandleCentre, OutDrawElements, MarkerLayer, AllottedGeometry, MyCullingRect, NodeColor);
	// 	}
	// }

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
		ActActionSequence::ENotifyStateHandleHit MarkerHit = DurationHandleHitTest(LastMouseDownPosition);
		if (MarkerHit == ActActionSequence::ENotifyStateHandleHit::Start || MarkerHit == ActActionSequence::ENotifyStateHandleHit::End)
		{
			bDragOnMarker = true;
			bBeingDragged = false;
			CurrentDragHandle = MarkerHit;
		}
	}

	return OnNodeDragStarted.Execute(SharedThis(this), MouseEvent, ScreenNodePosition, bDragOnMarker);
}


FVector2D SActActionSequenceNotifyNode::ComputeDesiredSize(float) const
{
	return WidgetSize;
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
	ActActionSequence::FActActionTrackAreaArgs& ActActionTrackAreaArgs = ActActionTrackAreaSlot.Pin()->GetActActionTrackAreaArgs();
	FTrackScaleInfo ScaleInfo(ActActionTrackAreaArgs.ViewInputMin.Get(), ActActionTrackAreaArgs.ViewInputMax.Get(), 0, 0, AllottedGeometry.Size);

	// Cache the geometry information, the allotted geometry is the same size as the track.
	CachedAllottedGeometrySize = AllottedGeometry.Size * AllottedGeometry.Scale;

	NotifyTimePositionX = ScaleInfo.InputToLocalX(ActActionTrackAreaSlot.Pin()->GetTime());
	NotifyDurationSizeX = ScaleInfo.PixelsPerInput * ActActionTrackAreaSlot.Pin()->GetDuration();

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
	float NotifyHeight = ActActionTrackAreaSlot.Pin()->NotifyHeight;
	WidgetSize = bDrawTooltipToRight ? FVector2D((NotifyDurationSizeX > 0.0f ? NotifyDurationSizeX : FMath::Max(LabelWidth, NotifyDurationSizeX)), NotifyHeight) : FVector2D((LabelWidth + NotifyDurationSizeX), NotifyHeight);
	WidgetSize.X += NotifyHandleBoxWidth;
	// Widget position of the notify marker
	NotifyScrubHandleCentre = bDrawTooltipToRight ? NotifyHandleBoxWidth / 2.f : LabelWidth;
}

void SActActionSequenceNotifyNode::DrawScrubHandle(float ScrubHandleCentre, FSlateWindowElementList& OutDrawElements, int32 ScrubHandleID, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FLinearColor NodeColour) const
{
	float NotifyHeight = ActActionTrackAreaSlot.Pin()->NotifyHeight;
	FVector2D ScrubHandlePosition(ScrubHandleCentre - ScrubHandleSize.X / 2.0f, (NotifyHeight - ScrubHandleSize.Y) / 2.f);
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
	float NotifyHeight = ActActionTrackAreaSlot.Pin()->NotifyHeight;
	FVector2D MarkerPosition;
	FVector2D MarkerSize = AlignmentMarkerSize;

	if (Offset < 0.f)
	{
		MarkerPosition.Set(HandleCentre - AlignmentMarkerSize.X, (NotifyHeight - AlignmentMarkerSize.Y) / 2.f);
	}
	else
	{
		MarkerPosition.Set(HandleCentre + AlignmentMarkerSize.X, (NotifyHeight - AlignmentMarkerSize.Y) / 2.f);
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


ActActionSequence::ENotifyStateHandleHit SActActionSequenceNotifyNode::DurationHandleHitTest(const FVector2D& CursorTrackPosition) const
{
	float NotifyHeight = ActActionTrackAreaSlot.Pin()->NotifyHeight;
	float NotifyHeightOffset = ActActionTrackAreaSlot.Pin()->NotifyHeightOffset;
	ActActionSequence::ENotifyStateHandleHit MarkerHit = ActActionSequence::ENotifyStateHandleHit::None;

	// Make sure this node has a duration box (meaning it is a state node)
	if (NotifyDurationSizeX > 0.0f)
	{
		// Test for mouse inside duration box with handles included
		float ScrubHandleHalfWidth = ScrubHandleSize.X / 2.0f;

		// Position and size of the notify node including the scrub handles
		FVector2D NotifyNodePosition(NotifyScrubHandleCentre - ScrubHandleHalfWidth, 0.0f);
		FVector2D NotifyNodeSize(NotifyDurationSizeX + ScrubHandleHalfWidth * 2.0f, NotifyHeight);

		FVector2D MouseRelativePosition(CursorTrackPosition - FVector2D(WidgetX, NotifyHeightOffset));

		if (MouseRelativePosition > NotifyNodePosition && MouseRelativePosition < (NotifyNodePosition + NotifyNodeSize))
		{
			// Definitely inside the duration box, need to see which handle we hit if any
			if (MouseRelativePosition.X <= (NotifyNodePosition.X + ScrubHandleSize.X))
			{
				// Left Handle
				MarkerHit = ActActionSequence::ENotifyStateHandleHit::Start;
			}
			else if (MouseRelativePosition.X >= (NotifyNodePosition.X + NotifyNodeSize.X - ScrubHandleSize.X))
			{
				// Right Handle
				MarkerHit = ActActionSequence::ENotifyStateHandleHit::End;
			}
		}
	}

	return MarkerHit;
}

/** @return the Node's position within the track */
FVector2D SActActionSequenceNotifyNode::GetWidgetPosition() const
{
	return FVector2D(WidgetX, ActActionTrackAreaSlot.Pin()->NotifyHeightOffset);
}
