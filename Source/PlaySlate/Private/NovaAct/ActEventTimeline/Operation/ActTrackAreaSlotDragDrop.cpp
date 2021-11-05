#include "ActTrackAreaSlotDragDrop.h"

#include "SCurveEditor.h"
#include "Common/NovaConst.h"
#include "Common/NovaStruct.h"
#include "Common/NovaDataBinding.h"
#include "Common/NovaStaticFunction.h"
#include "NovaAct/ActEventTimeline/Image/PoolWidgetTypes/ActPoolWidgetNotifyWidget.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActNotifyPoolLaneWidget.h"
#include "NovaAct/ActEventTimeline/Image/Subs/ActNotifyPoolNotifyNodeWidget.h"


using namespace NovaStruct;
using namespace NovaConst;

#define LOCTEXT_NAMESPACE "NovaAct"

FActTrackAreaSlotDragDrop::FActTrackAreaSlotDragDrop(float& InCurrentDragXPosition)
	: CurrentDragXPosition(InCurrentDragXPosition),
	  TrackSpan(0) { }

void FActTrackAreaSlotDragDrop::OnDrop(bool bDropWasHandled, const FPointerEvent& MouseEvent)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!bDropWasHandled && DB)
	{
		UAnimSequenceBase* AnimSequence = *DB->GetData();
		int32 NumNodes = SelectedNodes.Num();

		const FScopedTransaction Transaction(NumNodes > 0 ?
			                                     LOCTEXT("MoveNotifiesEvent", "Move Anim Notifies") :
			                                     LOCTEXT("MoveNotifyEvent", "Move Anim Notify"));
		AnimSequence->Modify();

		for (int32 CurrentNode = 0; CurrentNode < NumNodes; ++CurrentNode)
		{
			TSharedPtr<SActNotifyPoolNotifyNodeWidget> Node = SelectedNodes[CurrentNode];
			float NodePositionOffset = NodesXOffset[CurrentNode];
			TSharedPtr<FActDragDropLaneClampInfo> ClampInfo = GetLaneClampInfo(Node->ScreenPosition);
			TSharedPtr<SActNotifyPoolLaneWidget> LaneWidget = ClampInfo->Lane;
			float LocalX = LaneWidget->GetCachedGeometry().AbsoluteToLocal(Node->ScreenPosition + NodePositionOffset).X;
			float SnapTime = Node->LastSnappedTime;
			float Time = SnapTime != -1.0f ? SnapTime : LaneWidget->CachedScaleInfo->LocalXToInput(LocalX);
			FAnimNotifyEvent* AnimNotifyEvent = Node->AnimNotifyEvent;
			int32 TrackIndex = LaneWidget->LaneIndex;
			float EventDuration = AnimNotifyEvent->GetDuration();
			AnimNotifyEvent->Link(AnimSequence, Time, AnimNotifyEvent->GetSlotIndex());
			AnimNotifyEvent->RefreshTriggerOffset(AnimSequence->CalculateOffsetForNotify(AnimNotifyEvent->GetTime()));
			if (EventDuration > 0.0f)
			{
				AnimNotifyEvent->EndLink.Link(AnimSequence, AnimNotifyEvent->GetTime() + EventDuration, AnimNotifyEvent->GetSlotIndex());
				AnimNotifyEvent->RefreshEndTriggerOffset(AnimSequence->CalculateOffsetForNotify(AnimNotifyEvent->EndLink.GetTime()));
			}
			else
			{
				AnimNotifyEvent->EndTriggerTimeOffset = 0.0f;
			}

			AnimNotifyEvent->TrackIndex = TrackIndex;
			Node->bBeingDragged = false;
		}

		AnimSequence->PostEditChange();
		AnimSequence->MarkPackageDirty();

		NovaDB::Trigger("ActImageTrack/Refresh");
	}

	FDragDropOperation::OnDrop(bDropWasHandled, MouseEvent);
}

void FActTrackAreaSlotDragDrop::OnDragged(const FDragDropEvent& DragDropEvent)
{
	auto DB = GetDataBinding(UAnimSequenceBase**, "ActAnimation/AnimSequence");
	if (!DB)
	{
		return;
	}
	NodeGroupPosition = DragDropEvent.GetScreenSpacePosition() + DragOffset;

	TSharedPtr<FActDragDropLaneClampInfo> SelectionPositionClampInfo = GetLaneClampInfo(DragDropEvent.GetScreenSpacePosition());
	if ((SelectionPositionClampInfo->Lane->LaneIndex + TrackSpan) >= ClampInfos.Num())
	{
		// Our selection has moved off the bottom of the notify panel, adjust the clamping information to keep it on the panel
		SelectionPositionClampInfo = ClampInfos[ClampInfos.Num() - TrackSpan - 1];
	}

	const FGeometry& TrackGeom = SelectionPositionClampInfo->Lane->GetCachedGeometry();
	const FTrackScaleInfo& TrackScaleInfo = *SelectionPositionClampInfo->Lane->CachedScaleInfo;

	FVector2D SelectionBeginPosition = TrackGeom.LocalToAbsolute(
		TrackGeom.AbsoluteToLocal(NodeGroupPosition) + BeginSelectedNode->GetNotifyPositionOffset());

	float LocalTrackMin = TrackScaleInfo.InputToLocalX(0.0f);
	UAnimSequenceBase* AnimSequence = *DB->GetData();
	float PlayLength = AnimSequence->GetPlayLength();
	float LocalTrackMax = TrackScaleInfo.InputToLocalX(PlayLength);
	float LocalTrackWidth = LocalTrackMax - LocalTrackMin;

	// Tracks the movement amount to apply to the selection due to a snap.
	float SnapMovement = 0.0f;
	// Clamp the selection into the track
	float SelectionBeginLocalPositionX = TrackGeom.AbsoluteToLocal(SelectionBeginPosition).X;
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

	SelectionBeginPosition.X = TrackGeom.LocalToAbsolute(FVector2D(SelectionBeginLocalPositionX, 0.0f)).X;

	// Handle node snaps
	bool bSnapped = false;
	for (int32 NodeIdx = 0; NodeIdx < SelectedNodes.Num() && !bSnapped; ++NodeIdx)
	{
		TSharedPtr<SActNotifyPoolNotifyNodeWidget> CurrentNode = SelectedNodes[NodeIdx];

		// Clear off any snap time currently stored
		CurrentNode->LastSnappedTime = -1.0f;

		TSharedPtr<FActDragDropLaneClampInfo> NodeClamp = GetLaneClampInfo(CurrentNode->ScreenPosition);

		FVector2D EventPosition = SelectionBeginPosition + FVector2D(TrackScaleInfo.PixelsPerInput * NodesTimeOffset[NodeIdx], 0.0f);

		// Look for a snap on the first scrub handle
		FVector2D TrackNodePos = TrackGeom.AbsoluteToLocal(EventPosition);
		float SequenceEnd = TrackScaleInfo.InputToLocalX(PlayLength);

		// Always clamp the Y to the current track
		SelectionBeginPosition.Y = SelectionPositionClampInfo->LanePos - 1.0f;

		float SnapX = GetSnapPosition(NodeClamp.ToSharedRef(), TrackNodePos.X, PlayLength, bSnapped);
		if (FAnimNotifyEvent* CurrentEvent = CurrentNode->AnimNotifyEvent)
		{
			if (bSnapped)
			{
				EAnimEventTriggerOffsets::Type Offset;
				if (SnapX == 0.0f || SnapX == SequenceEnd)
				{
					Offset = SnapX > 0.0f ? EAnimEventTriggerOffsets::OffsetBefore : EAnimEventTriggerOffsets::OffsetAfter;
				}
				else
				{
					Offset = (SnapX < TrackNodePos.X) ? EAnimEventTriggerOffsets::OffsetAfter : EAnimEventTriggerOffsets::OffsetBefore;
				}

				CurrentEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);
				CurrentNode->LastSnappedTime = TrackScaleInfo.LocalXToInput(SnapX);

				if (SnapMovement == 0.0f)
				{
					SnapMovement = SnapX - TrackNodePos.X;
					TrackNodePos.X = SnapX;
				}
			}
			else
			{
				CurrentEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
			}

			// If we didn't snap the beginning of the node, attempt to snap the end
			if (CurrentNode.IsValid() && CurrentEvent->GetDuration() > 0 && !bSnapped)
			{
				FVector2D TrackNodeEndPos = TrackNodePos + CurrentNode->DurationSizeX;
				SnapX = GetSnapPosition(SelectionPositionClampInfo.ToSharedRef(), TrackNodeEndPos.X, PlayLength, bSnapped);

				// Only attempt to snap if the node will fit on the track
				if (SnapX >= CurrentNode->DurationSizeX)
				{
					// Only need to check the end of the sequence here; end handle can't hit the beginning
					EAnimEventTriggerOffsets::Type Offset = SnapX == SequenceEnd ?
						                                        (EAnimEventTriggerOffsets::OffsetBefore) :
						                                        ((SnapX < TrackNodeEndPos.X) ?
							                                         EAnimEventTriggerOffsets::OffsetAfter :
							                                         EAnimEventTriggerOffsets::OffsetBefore);
					CurrentEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(Offset);

					if (SnapMovement == 0.0f)
					{
						SnapMovement = SnapX - TrackNodeEndPos.X;
						CurrentNode->LastSnappedTime = TrackScaleInfo.LocalXToInput(SnapX) - CurrentEvent->GetDuration();
					}
				}
				else
				{
					// Remove any trigger time if we can't fit the node in.
					CurrentEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::NoOffset);
				}
			}
		}
	}

	SelectionBeginPosition.X += SnapMovement;

	CurrentDragXPosition = TrackGeom.AbsoluteToLocal(FVector2D(SelectionBeginPosition.X, 0.0f)).X;

	CursorDecoratorWindow->MoveWindowTo(
		TrackGeom.LocalToAbsolute(TrackGeom.AbsoluteToLocal(SelectionBeginPosition) - SelectedNodes[0]->GetNotifyPositionOffset()));
	NodeGroupPosition = SelectionBeginPosition;

	//scroll view
	float LocalMouseXPos = TrackGeom.AbsoluteToLocal(DragDropEvent.GetScreenSpacePosition()).X;
	float LocalViewportMin = 0.0f;
	float LocalViewportMax = TrackGeom.GetLocalSize().X;
	if (LocalMouseXPos < LocalViewportMin && LocalViewportMin > LocalTrackMin - 10.0f)
	{
		float ScreenDelta = FMath::Max(LocalMouseXPos - LocalViewportMin, -10.0f);
		NovaStaticFunction::OnViewRangePan(ScreenDelta, FVector2D(LocalTrackWidth, 1.f));
	}
	else if (LocalMouseXPos > LocalViewportMax && LocalViewportMax < LocalTrackMax + 10.0f)
	{
		float ScreenDelta = FMath::Max(LocalMouseXPos - LocalViewportMax, 10.0f);
		NovaStaticFunction::OnViewRangePan(ScreenDelta, FVector2D(LocalTrackWidth, 1.f));
	}

	if (DragDropEvent.IsShiftDown())
	{
		auto ActEventTimelineArgsDB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
		const FFrameTime FrameTime = FFrameTime::FromDecimal(
			TrackScaleInfo.LocalXToInput(CurrentDragXPosition) * ActEventTimelineArgs->TickResolution.AsDecimal());
		*ActEventTimelineArgs->CurrentTime = FrameTime;
		NovaDB::Trigger("ActEventTimelineArgs/CurrentTime");
	}
	auto PoolNotifyDB = GetDataBindingSP(SActPoolWidgetNotifyWidget, "ActPoolNotify");
	if (PoolNotifyDB)
	{
		PoolNotifyDB->GetData()->OnNotifyNodesBeingDraggedStatusBarMessage();
	}
}

TSharedPtr<SWidget> FActTrackAreaSlotDragDrop::GetDefaultDecorator() const
{
	return Decorator;
}

TSharedRef<FActTrackAreaSlotDragDrop> FActTrackAreaSlotDragDrop::New(
	TSharedPtr<SActPoolWidgetNotifyWidget> PoolNotifyWidget,
	TArray<TSharedPtr<SActNotifyPoolNotifyNodeWidget>> NotifyNodes,
	TSharedPtr<SWidget> Decorator,
	const FVector2D& CursorPosition,
	const FVector2D& SelectionScreenPosition,
	const FVector2D& SelectionSize,
	float& CurrentDragXPosition)
{
	TSharedRef<FActTrackAreaSlotDragDrop> Operation = MakeShareable(new FActTrackAreaSlotDragDrop(CurrentDragXPosition));
	{
		Operation->NodeGroupPosition = SelectionScreenPosition;
		Operation->NodeGroupSize = SelectionSize;
		Operation->DragOffset = SelectionScreenPosition - CursorPosition;
		Operation->Decorator = Decorator;
		Operation->SelectedNodes = NotifyNodes;
		Operation->TrackSpan = NotifyNodes.Last()->AnimNotifyEvent->TrackIndex - NotifyNodes[0]->AnimNotifyEvent->TrackIndex;
	}

	// Calculate offsets for the selected nodes
	float BeginTime = MAX_flt;
	for (TSharedPtr<SActNotifyPoolNotifyNodeWidget>& NotifyNode : NotifyNodes)
	{
		float NotifyTime = NotifyNode->AnimNotifyEvent->GetTime();

		if (NotifyTime < BeginTime)
		{
			BeginTime = NotifyTime;
			Operation->BeginSelectedNode = NotifyNode;
		}
	}

	// Initialise node data
	for (TSharedPtr<SActNotifyPoolNotifyNodeWidget> NotifyNode : NotifyNodes)
	{
		float NotifyTime = NotifyNode->AnimNotifyEvent->GetTime();

		NotifyNode->LastSnappedTime = -1.0f;
		Operation->NodesTimeOffset.Add(NotifyTime - BeginTime);
		Operation->NodesXOffset.Add(NotifyNode->GetNotifyPositionOffset().X);
	}
	for (TSharedPtr<SActNotifyPoolLaneWidget> Lane : PoolNotifyWidget->NotifyLanes)
	{
		TSharedPtr<FActDragDropLaneClampInfo> Info = MakeShareable(new FActDragDropLaneClampInfo);
		{
			Info->Lane = Lane;
			const FGeometry& CachedGeometry = Lane->GetCachedGeometry();
			Info->LanePos = CachedGeometry.AbsolutePosition.Y;
			Info->LaneSnapTestPos = Info->LanePos + (CachedGeometry.Size.Y / 2);
		};
		Operation->ClampInfos.Add(Info);
	}

	Operation->Construct();
	Operation->CursorDecoratorWindow->SetOpacity(0.5f);

	return Operation;
}

float FActTrackAreaSlotDragDrop::GetSnapPosition(const TSharedRef<FActDragDropLaneClampInfo>& InClampInfo,
                                                 float WidgetSpaceNotifyPosition,
                                                 float PlayLength,
                                                 bool& bOutSnapped)
{
	const FTrackScaleInfo& ScaleInfo = *InClampInfo->Lane->CachedScaleInfo;

	const float MaxSnapDist = 5.f;

	float CurrentMinSnapDest = MaxSnapDist;
	float SnapPosition = ScaleInfo.LocalXToInput(WidgetSpaceNotifyPosition);
	bOutSnapped = !FSlateApplication::Get().GetModifierKeys().IsControlDown() &&
		NovaStaticFunction::OnSnapTime(SnapPosition, MaxSnapDist / ScaleInfo.PixelsPerInput, TArrayView<const FName>());
	bOutSnapped = false;
	SnapPosition = ScaleInfo.InputToLocalX(SnapPosition);

	float WidgetSpaceStartPosition = ScaleInfo.InputToLocalX(0.0f);
	float WidgetSpaceEndPosition = ScaleInfo.InputToLocalX(PlayLength);

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

TSharedPtr<FActDragDropLaneClampInfo> FActTrackAreaSlotDragDrop::GetLaneClampInfo(const FVector2D NodePos)
{
	int32 ClampInfoIndex = 0;
	int32 SmallestNodeTrackDist = MAX_int32;
	for (int32 i = 0; i < ClampInfos.Num(); ++i)
	{
		int32 Dist = FMath::Abs(ClampInfos[i]->LaneSnapTestPos - NodePos.Y);
		if (Dist < SmallestNodeTrackDist)
		{
			SmallestNodeTrackDist = Dist;
			ClampInfoIndex = i;
		}
	}
	return ClampInfos[ClampInfoIndex];
}

#undef LOCTEXT_NAMESPACE
