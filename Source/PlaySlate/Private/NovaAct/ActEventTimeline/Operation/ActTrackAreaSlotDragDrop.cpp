#include "ActTrackAreaSlotDragDrop.h"

#include "SCurveEditor.h"
#include "NovaAct/Widgets/ActEventTimeline/SequenceNodeTree/Subs/ActActionSequenceNotifyNode.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionTrackAreaSlot.h"


#define LOCTEXT_NAMESPACE "NovaAct"

FActTrackAreaSlotDragDrop::FActTrackAreaSlotDragDrop(float& InCurrentDragXPosition)
	: CurrentDragXPosition(InCurrentDragXPosition),
	  SnapTime(-1.f),
	  SelectionTimeLength(0.0f),
	  TrackSpan(0),
	  NodeTimeOffset(0),
	  NodeTime(0),
	  NodeXOffset(0) { }

void FActTrackAreaSlotDragDrop::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	if (bDropWasHandled == false)
	{
		// int32 NumNodes = SelectedNodes.Num();

		const FScopedTransaction Transaction(LOCTEXT("MoveNotifiesEvent", "Move Anim Notifies"));
		// Sequence->Modify();
		// TSharedPtr<SActActionSequenceNotifyNode> Node = SelectedNodes[CurrentNode];
		// float NodePositionOffset = NodeXOffsets[CurrentNode];
		// const FTrackClampInfo& ClampInfo = GetTrackClampInfo(Node->GetScreenPosition());
		ensure(SelectedNode.IsValid());

		// if (SelectedNode->NodeObjectInterface->GetType() == ENodeObjectTypes::SYNC_MARKER)
		// {
		// 	UBlendSpaceBase::UpdateBlendSpacesUsingAnimSequence(Sequence);
		// }

		// float LocalX = SelectedNode->CachedTrackGeometry.AbsoluteToLocal(SelectedNode->ScreenPosition + NodeXOffset).X;
		// float LastSnappedTime = SelectedNode->LastSnappedTime;
		// ActActionSequence::FActActionTrackAreaArgs& TrackAreaArgs = SelectedNode->GetActActionTrackAreaSlot()->GetActActionTrackAreaArgs();
		// FTrackScaleInfo TrackScaleInfo = FTrackScaleInfo(TrackAreaArgs.ViewInputMin.Get(), TrackAreaArgs.ViewInputMax.Get(), 0.f, 0.f, SelectedNode->CachedTrackGeometry.GetLocalSize());
		// ** TODO: 使用这个时间
		// float Time = SnapTime != -1.0f ? SnapTime : TrackScaleInfo.LocalXToInput(LocalX);
		// SelectedNode->NodeObjectInterface->HandleDrop(Sequence, Time, TrackIndex);
		// SelectedNode->HandleNodeDrop(SelectedNode, NodeXOffset);
		SelectedNode->bBeingDragged = false;

		// for(int32 CurrentNode = 0 ; CurrentNode < NumNodes ; ++CurrentNode)
		// {
		// }

		// Sequence->PostEditChange();
		// Sequence->MarkPackageDirty();

		// OnUpdatePanel.ExecuteIfBound();
	}

	FDragDropOperation::OnDrop(bDropWasHandled, MouseEvent);
}

void FActTrackAreaSlotDragDrop::OnDragged(const FDragDropEvent& DragDropEvent)
{
	// Reset snapped node pointer
	// SnappedNode = NULL;

	NodeGroupPosition = DragDropEvent.GetScreenSpacePosition() + DragOffset;

	// FTrackClampInfo* SelectionPositionClampInfo = &GetTrackClampInfo(DragDropEvent.GetScreenSpacePosition());
	// if ((SelectionPositionClampInfo->NotifyTrack->GetTrackIndex() + TrackSpan) >= ClampInfos.Num())
	// {
	// 	// Our selection has moved off the bottom of the notify panel, adjust the clamping information to keep it on the panel
	// 	SelectionPositionClampInfo = &ClampInfos[ClampInfos.Num() - TrackSpan - 1];
	// }
	const FGeometry& TrackGeometry = SelectedNode->CachedTrackGeometry;
	FActActionTrackAreaArgs& TrackAreaArgs = SelectedNode->GetActActionTrackAreaSlot()->GetActActionTrackAreaArgs();
	float PlayLength = TrackAreaArgs.GetPlayLength();
	const FTrackScaleInfo& TrackScaleInfo = FTrackScaleInfo(TrackAreaArgs.ViewInputMin.Get(), TrackAreaArgs.ViewInputMax.Get(), 0.f, 0.f, SelectedNode->CachedTrackGeometry.GetLocalSize());

	FVector2D LocalCoordinate = TrackGeometry.AbsoluteToLocal(NodeGroupPosition) + SelectedNode->GetNotifyPositionOffset();
	FVector2D SelectionBeginPosition = TrackGeometry.LocalToAbsolute(LocalCoordinate);

	float LocalTrackMin = TrackScaleInfo.InputToLocalX(0.0f);
	float LocalTrackMax = TrackScaleInfo.InputToLocalX(PlayLength);
	// float LocalTrackWidth = LocalTrackMax - LocalTrackMin;

	// Tracks the movement amount to apply to the selection due to a snap.
	float SnapMovement = 0.0f;
	// Clamp the selection into the track
	float SelectionBeginLocalPositionX = TrackGeometry.AbsoluteToLocal(SelectionBeginPosition).X;
	const float ClampedEnd = FMath::Clamp(SelectionBeginLocalPositionX + NodeGroupSize.X, LocalTrackMin, LocalTrackMax);
	const float ClampedBegin = FMath::Clamp(SelectionBeginLocalPositionX, LocalTrackMin, LocalTrackMax);
	if (ClampedBegin > SelectionBeginLocalPositionX)
	{
		SelectionBeginLocalPositionX = ClampedBegin;
	}
	else if (ClampedEnd < SelectionBeginLocalPositionX + NodeGroupSize.X)
	{
		SelectionBeginLocalPositionX = ClampedEnd - NodeGroupSize.X;
	}

	SelectionBeginPosition.X = TrackGeometry.LocalToAbsolute(FVector2D(SelectionBeginLocalPositionX, 0.0f)).X;

	// Handle node snaps
	// bool bSnapped = false;
	// for (int32 NodeIdx = 0; NodeIdx < SelectedNodes.Num() && !bSnapped; ++NodeIdx)
	// {
	// TSharedPtr<SAnimNotifyNode> CurrentNode = SelectedNodes[NodeIdx];
	//
	// // Clear off any snap time currently stored
	// CurrentNode->ClearLastSnappedTime();
	//
	// const FTrackClampInfo& NodeClamp = GetTrackClampInfo(CurrentNode->GetScreenPosition());

	// FVector2D EventPosition = SelectionBeginPosition + FVector2D(TrackScaleInfo.PixelsPerInput * NodeTimeOffset, 0.0f);

	// Look for a snap on the first scrub handle
	// FVector2D TrackNodePos = TrackGeometry.AbsoluteToLocal(EventPosition);
	// const FVector2D OriginalNodePosition = TrackNodePos;
	// float SequenceEnd = TrackScaleInfo.InputToLocalX(PlayLength);

	// Always clamp the Y to the current track
	// SelectionBeginPosition.Y = SelectionPositionClampInfo->TrackPos - 1.0f;

	// float SnapX = GetSnapPosition(TrackScaleInfo, TrackNodePos.X, bSnapped, PlayLength);
	// if (FAnimNotifyEvent* CurrentEvent = CurrentNode->NodeObjectInterface->GetNotifyEvent())
	// {
	// 	if (bSnapped)
	// 	{
	// 		EAnimEventTriggerOffsets::Type Offset = EAnimEventTriggerOffsets::NoOffset;
	// 		if (SnapX == 0.0f || SnapX == SequenceEnd)
	// 		{
	// 			Offset = SnapX > 0.0f ? EAnimEventTriggerOffsets::OffsetBefore : EAnimEventTriggerOffsets::OffsetAfter;
	// 		}
	// 		else
	// 		{
	// 			Offset = (SnapX < TrackNodePos.X) ? EAnimEventTriggerOffsets::OffsetAfter : EAnimEventTriggerOffsets::OffsetBefore;
	// 		}
	//
	// 		CurrentEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);
	// 		CurrentNode->SetLastSnappedTime(TrackScaleInfo.LocalXToInput(SnapX));
	//
	// 		if (SnapMovement == 0.0f)
	// 		{
	// 			SnapMovement = SnapX - TrackNodePos.X;
	// 			TrackNodePos.X = SnapX;
	// 			SnapTime = TrackScaleInfo.LocalXToInput(SnapX);
	// 			// SnappedNode = CurrentNode;
	// 		}
	// 		EventPosition = NodeClamp.NotifyTrack->GetCachedGeometry().LocalToAbsolute(TrackNodePos);
	// 	}
	// 	else
	// 	{
	// 		CurrentEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
	// 	}
	//
	// 	if (CurrentNode.IsValid() && CurrentEvent->GetDuration() > 0)
	// 	{
	// 		// If we didn't snap the beginning of the node, attempt to snap the end
	// 		if (!bSnapped)
	// 		{
	// 			FVector2D TrackNodeEndPos = TrackNodePos + CurrentNode->GetDurationSize();
	// 			SnapX = GetSnapPosition(*SelectionPositionClampInfo, TrackNodeEndPos.X, bSnapped);
	//
	// 			// Only attempt to snap if the node will fit on the track
	// 			if (SnapX >= CurrentNode->GetDurationSize())
	// 			{
	// 				EAnimEventTriggerOffsets::Type Offset = EAnimEventTriggerOffsets::NoOffset;
	// 				if (SnapX == SequenceEnd)
	// 				{
	// 					// Only need to check the end of the sequence here; end handle can't hit the beginning
	// 					Offset = EAnimEventTriggerOffsets::OffsetBefore;
	// 				}
	// 				else
	// 				{
	// 					Offset = (SnapX < TrackNodeEndPos.X) ? EAnimEventTriggerOffsets::OffsetAfter : EAnimEventTriggerOffsets::OffsetBefore;
	// 				}
	// 				CurrentEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);
	//
	// 				if (SnapMovement == 0.0f)
	// 				{
	// 					SnapMovement = SnapX - TrackNodeEndPos.X;
	// 					SnapTime = TrackScaleInfo.LocalXToInput(SnapX) - CurrentEvent->GetDuration();
	// 					CurrentNode->SetLastSnappedTime(SnapTime);
	// 					// SnappedNode = CurrentNode;
	// 				}
	// 			}
	// 			else
	// 			{
	// 				// Remove any trigger time if we can't fit the node in.
	// 				CurrentEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
	// 			}
	// 		}
	// 	}
	// }
	// }

	SelectionBeginPosition.X += SnapMovement;

	CurrentDragXPosition = TrackGeometry.AbsoluteToLocal(FVector2D(SelectionBeginPosition.X, 0.0f)).X;

	CursorDecoratorWindow->MoveWindowTo(TrackGeometry.LocalToAbsolute(TrackGeometry.AbsoluteToLocal(SelectionBeginPosition) - SelectedNode->GetNotifyPositionOffset()));
	NodeGroupPosition = SelectionBeginPosition;

	//scroll view
	// float LocalMouseXPos = TrackGeometry.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()).X;
	// float LocalViewportMin = 0.0f;
	// float LocalViewportMax = TrackGeometry.GetLocalSize().X;
	// if (LocalMouseXPos < LocalViewportMin && LocalViewportMin > LocalTrackMin - 10.0f)
	// {
	// 	float ScreenDelta = FMath::Max(LocalMouseXPos - LocalViewportMin, -10.0f);
	// 	// RequestTrackPan.Execute(ScreenDelta, FVector2D(LocalTrackWidth, 1.f));
	// }
	// else if (LocalMouseXPos > LocalViewportMax && LocalViewportMax < LocalTrackMax + 10.0f)
	// {
	// 	float ScreenDelta = FMath::Max(LocalMouseXPos - LocalViewportMax, 10.0f);
	// 	// RequestTrackPan.Execute(ScreenDelta, FVector2D(LocalTrackWidth, 1.f));
	// }

	// OnNodesBeingDragged.ExecuteIfBound(SelectedNodes, DragDropEvent, CurrentDragXPosition, TrackScaleInfo.LocalXToInput(CurrentDragXPosition));
}

//
// FActTrackAreaSlotDragDrop::FNovaTrackClampInfo& FActTrackAreaSlotDragDrop::GetTrackClampInfo(const FVector2D NodePos)
// {
// 	int32 ClampInfoIndex = 0;
// 	int32 SmallestNodeTrackDist = FMath::Abs(ClampInfos[0].TrackSnapTestPos - NodePos.Y);
// 	for(int32 i = 0; i < ClampInfos.Num(); ++i)
// 	{
// 		int32 Dist = FMath::Abs(ClampInfos[i].TrackSnapTestPos - NodePos.Y);
// 		if(Dist < SmallestNodeTrackDist)
// 		{
// 			SmallestNodeTrackDist = Dist;
// 			ClampInfoIndex = i;
// 		}
// 	}
// 	return ClampInfos[ClampInfoIndex];
// 	
// }

TSharedRef<FActTrackAreaSlotDragDrop> FActTrackAreaSlotDragDrop::New(TSharedRef<SActActionSequenceNotifyNode> NotifyNode,
                                                                     TSharedPtr<SWidget> Decorator,
                                                                     const FVector2D& CursorPosition,
                                                                     const FVector2D& SelectionScreenPosition,
                                                                     const FVector2D& SelectionSize,
                                                                     float& CurrentDragXPosition)
{
	TSharedRef<FActTrackAreaSlotDragDrop> Operation = MakeShareable(new FActTrackAreaSlotDragDrop(CurrentDragXPosition));
	Operation->NodeGroupPosition = SelectionScreenPosition;
	Operation->NodeGroupSize = SelectionSize;
	Operation->DragOffset = SelectionScreenPosition - CursorPosition;
	Operation->Decorator = Decorator;
	Operation->SelectedNode = NotifyNode;
	Operation->TrackSpan = 0;

	// Calculate offsets for the selected nodes
	const FActActionTrackAreaArgs& TrackAreaArgs = NotifyNode->GetActActionTrackAreaSlot()->GetActActionTrackAreaArgs();
	float BeginTime = TrackAreaArgs.GetBeginTime();
	float EndTime = TrackAreaArgs.GetEndTime();

	// Initialise node data
	Operation->NodeTimeOffset = EndTime - BeginTime;
	Operation->NodeTime = EndTime;
	Operation->NodeXOffset = NotifyNode->GetNotifyPositionOffset().X;
	// Calculate the time length of the selection. Because it is possible to have states
	// with arbitrary durations we need to search all of the nodes and find the furthest
	// possible point
	Operation->SelectionTimeLength = FMath::Max(Operation->SelectionTimeLength, EndTime - BeginTime);
	Operation->Construct();

	// for (int32 i = 0; i < NotifyTracks.Num(); ++i)
	// {
	// 	FTrackClampInfo Info;
	// 	Info.NotifyTrack = NotifyTracks[i];
	// 	const FGeometry& CachedGeometry = Info.NotifyTrack->GetCachedGeometry();
	// 	Info.TrackPos = CachedGeometry.AbsolutePosition.Y;
	// 	Info.TrackSnapTestPos = Info.TrackPos + (CachedGeometry.Size.Y / 2);
	// 	Operation->ClampInfos.Add(Info);
	// }

	Operation->CursorDecoratorWindow->SetOpacity(0.5f);
	return Operation;
}

float FActTrackAreaSlotDragDrop::GetSnapPosition(const FTrackScaleInfo& InScaleInfo, float WidgetSpaceNotifyPosition, bool& bOutSnapped, float PlayLength)
{
	const float MaxSnapDist = 5.f;

	float CurrentMinSnapDest = MaxSnapDist;
	float SnapPosition = InScaleInfo.LocalXToInput(WidgetSpaceNotifyPosition);
	// bOutSnapped = OnSnapPosition.IsBound() && !FSlateApplication::Get().GetModifierKeys().IsControlDown() && OnSnapPosition.Execute(SnapPosition, MaxSnapDist / ScaleInfo.PixelsPerInput, TArrayView<const FName>());
	SnapPosition = InScaleInfo.InputToLocalX(SnapPosition);

	float WidgetSpaceStartPosition = InScaleInfo.InputToLocalX(0.0f);
	float WidgetSpaceEndPosition = InScaleInfo.InputToLocalX(PlayLength);

	if (!bOutSnapped)
	{
		// Didn't snap to a bar, snap to the track bounds
		float SnapDistBegin = FMath::Abs(WidgetSpaceStartPosition - WidgetSpaceNotifyPosition);
		float SnapDistEnd = FMath::Abs(WidgetSpaceEndPosition - WidgetSpaceNotifyPosition);
		if (SnapDistBegin < CurrentMinSnapDest)
		{
			SnapPosition = WidgetSpaceStartPosition;
			bOutSnapped = true;
		}
		else if (SnapDistEnd < CurrentMinSnapDest)
		{
			SnapPosition = WidgetSpaceEndPosition;
			bOutSnapped = true;
		}
	}

	return SnapPosition;
}

#undef LOCTEXT_NAMESPACE
