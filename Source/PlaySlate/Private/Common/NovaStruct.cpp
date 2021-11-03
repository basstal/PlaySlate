#include "Common/NovaStruct.h"
#include "PlaySlate.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#define LOCTEXT_NAMESPACE "NovaAct"

namespace NovaStruct
{
	//~Begin FActEventTimelineArgs
	FActEventTimelineArgs::FActEventTimelineArgs()
		: ViewRange(new TRange<double>(0.0f, 1.0f)),
		  ClampRange(TRange<double>(0.0f, 1.0f)),
		  TickResolution(60, 1),
		  CurrentTime(new FFrameTime(0)),
		  PlaybackStatus(ENovaPlaybackType::Stopped),
		  AllowZoom(true) {}

	void FActEventTimelineArgs::SetViewRangeClamped(double NewRangeMin, double NewRangeMax)
	{
		UE_LOG(LogNovaAct, Log, TEXT("NewValue : (%f, %f)"), NewRangeMin, NewRangeMax);
		const double ClampBeginTime = ClampRange.GetLowerBoundValue();
		const double ClampEndTime = ClampRange.GetUpperBoundValue();
		const double TickInterval = TickResolution.AsInterval();
		// Clamp to the clamp range
		NewRangeMax = FMath::Clamp(NewRangeMax, TickInterval, ClampEndTime);
		NewRangeMin = FMath::Clamp(NewRangeMin, ClampBeginTime, ClampEndTime - TickInterval);
		// Clamp to a minimum size to avoid zero-sized or negative visible ranges
		const double MinVisibleTimeRange = FFrameNumber(1) / TickResolution;
		NewRangeMin = FMath::Clamp(NewRangeMin, ClampBeginTime, NewRangeMax - MinVisibleTimeRange);
		NewRangeMax = FMath::Clamp(NewRangeMax, NewRangeMin + MinVisibleTimeRange, ClampEndTime);
		ViewRange->SetLowerBoundValue(NewRangeMin);
		ViewRange->SetUpperBoundValue(NewRangeMax);
	}

	//~End FActEventTimelineArgs

	//~Begin FActSliderScrubRangeToScreen
	FActSliderScrubRangeToScreen::FActSliderScrubRangeToScreen(const TRange<double>& InViewInput, const FVector2D& InWidgetSize)
	{
		const double ViewInputRange = InViewInput.Size<double>();
		ViewStart = InViewInput.GetLowerBoundValue();
		PixelsPerInput = ViewInputRange > 0 ? (InWidgetSize.X / ViewInputRange) : 0;
	}

	double FActSliderScrubRangeToScreen::LocalXToInput(double ScreenX) const
	{
		return PixelsPerInput > 0 ? (ScreenX / PixelsPerInput) + ViewStart : ViewStart;
	}

	double FActSliderScrubRangeToScreen::InputToLocalX(double InputTime) const
	{
		return (InputTime - ViewStart) * PixelsPerInput;
	}

	//~End FActSliderScrubRangeToScreen

	TRange<FFrameTime> FActActionEvaluationRange::CalculateEvaluationRange(FFrameTime CurrentTime, FFrameTime PreviousTime, bool bInclusivePreviousTime)
	{
		if (CurrentTime == PreviousTime)
		{
			return TRange<FFrameTime>(CurrentTime);
		}
		else if (CurrentTime < PreviousTime)
		{
			return TRange<FFrameTime>(
				TRangeBound<FFrameTime>::Inclusive(CurrentTime),
				bInclusivePreviousTime ? TRangeBound<FFrameTime>::Inclusive(PreviousTime) : TRangeBound<FFrameTime>::Exclusive(PreviousTime)
			);
		}

		return TRange<FFrameTime>(
			bInclusivePreviousTime ? TRangeBound<FFrameTime>::Inclusive(PreviousTime) : TRangeBound<FFrameTime>::Exclusive(PreviousTime),
			TRangeBound<FFrameTime>::Inclusive(CurrentTime)
		);
	}

	TRange<FFrameNumber> FActActionEvaluationRange::TimeRangeToNumberRange(const TRange<FFrameTime>& InFrameTimeRange)
	{
		TRange<FFrameNumber> FrameNumberRange;
		TOptional<FFrameTime> UpperTime;
		if (!InFrameTimeRange.GetUpperBound().IsOpen())
		{
			UpperTime = InFrameTimeRange.GetUpperBoundValue();
			// Similar to adjusting the lower bound, if there's a sub frame on the upper bound, the frame number needs incrementing in order to evaluate keys in the sub frame
			if (UpperTime.GetValue().GetSubFrame() != 0.0f || InFrameTimeRange.GetUpperBound().IsInclusive())
			{
				UpperTime.GetValue().FrameNumber = UpperTime.GetValue().FrameNumber + 1;
			}
			FrameNumberRange.SetUpperBound(TRangeBound<FFrameNumber>::Exclusive(UpperTime.GetValue().FrameNumber));
		}

		if (!InFrameTimeRange.GetLowerBound().IsOpen())
		{
			FFrameTime LowerTime = InFrameTimeRange.GetLowerBoundValue();
			// If there is a sub frame on the start time, we're actually beyond that frame number, so it needs incrementing
			if (LowerTime.GetSubFrame() != 0.0f || InFrameTimeRange.GetLowerBound().IsExclusive())
			{
				LowerTime.FrameNumber = (!UpperTime.IsSet() || LowerTime.FrameNumber < UpperTime.GetValue().FrameNumber) ? LowerTime.FrameNumber + 1 : LowerTime.FrameNumber;
			}
			FrameNumberRange.SetLowerBound(TRangeBound<FFrameNumber>::Inclusive(LowerTime.FrameNumber));
		}


		return FrameNumberRange;
	}

	TRange<FFrameTime> FActActionEvaluationRange::NumberRangeToTimeRange(const TRange<FFrameNumber>& InFrameTimeRange)
	{
		TRange<FFrameTime> FrameTimeRange;

		if (!InFrameTimeRange.GetLowerBound().IsOpen())
		{
			const FFrameNumber FrameNumber = InFrameTimeRange.GetLowerBoundValue();

			FrameTimeRange.SetLowerBound(
				InFrameTimeRange.GetLowerBound().IsExclusive() ? TRangeBound<FFrameTime>::Exclusive(FrameNumber) : TRangeBound<FFrameTime>::Inclusive(FrameNumber)
			);
		}

		if (!InFrameTimeRange.GetUpperBound().IsOpen())
		{
			const FFrameNumber FrameNumber = InFrameTimeRange.GetUpperBoundValue();

			FrameTimeRange.SetUpperBound(
				InFrameTimeRange.GetUpperBound().IsExclusive() ? TRangeBound<FFrameTime>::Exclusive(FrameNumber) : TRangeBound<FFrameTime>::Inclusive(FrameNumber)
			);
		}

		return FrameTimeRange;
	}

	TRange<FFrameNumber> FActActionEvaluationRange::GetTraversedFrameNumberRange() const
	{
		TRange<FFrameNumber> FrameNumberRange;

		if (!EvaluationRange.GetLowerBound().IsOpen())
		{
			FFrameNumber StartFrame = EvaluationRange.GetLowerBoundValue().FloorToFrame();
			FrameNumberRange.SetLowerBound(TRangeBound<FFrameNumber>::Inclusive(StartFrame));
		}

		if (!EvaluationRange.GetUpperBound().IsOpen())
		{
			FFrameNumber EndFrame = EvaluationRange.GetUpperBoundValue().FloorToFrame() + 1;
			FrameNumberRange.SetUpperBound(TRangeBound<FFrameNumber>::Exclusive(EndFrame));
		}

		return FrameNumberRange;
	}

	void FActActionPlaybackPosition::SetTimeBase(FFrameRate InInputRate, FFrameRate InOutputRate)
	{
		// Move the current position if necessary
		if (InputRate.IsValid() && InputRate != InInputRate)
		{
			FFrameTime NewPosition = ConvertFrameTime(CurrentPosition, InputRate, InInputRate);
			// if (NewEvaluationType == EMovieSceneEvaluationType::FrameLocked)
			// {
			// 	NewPosition = NewPosition.FloorToFrame();
			// }

			Reset(NewPosition);
		}

		InputRate = InInputRate;
		OutputRate = InOutputRate;
		// EvaluationType = NewEvaluationType;
	}

	void FActActionPlaybackPosition::Reset(FFrameTime StartPos)
	{
		CurrentPosition = StartPos;
		PreviousPlayEvalPosition.Reset();
		LastRange.Reset();
	}

	FActActionEvaluationRange FActActionPlaybackPosition::JumpTo(FFrameTime NewPosition)
	{
		CheckInvariants();

		PreviousPlayEvalPosition.Reset();

		// Floor to the current frame number if running frame-locked
		// if (EvaluationType == EMovieSceneEvaluationType::FrameLocked)
		// {
		// 	InputPosition = InputPosition.FloorToFrame();
		// }

		// Assign the cached input values
		CurrentPosition = NewPosition;

		// Convert to output time-base
		FFrameTime OutputPosition = ConvertFrameTime(NewPosition, InputRate, OutputRate);

		LastRange = FActActionEvaluationRange(TRange<FFrameTime>(OutputPosition), OutputRate, ENovaPlayDirection::Forwards);
		return LastRange.GetValue();
	}

	FActActionEvaluationRange FActActionPlaybackPosition::PlayTo(FFrameTime NewPosition)
	{
		CheckInvariants();

		// Floor to the current frame number if running frame-locked
		// if (EvaluationType == EMovieSceneEvaluationType::FrameLocked)
		// {
		// 	InputPosition = InputPosition.FloorToFrame();
		// }

		// Convert to output time-base
		FFrameTime InputEvalPositionFrom = PreviousPlayEvalPosition.Get(CurrentPosition);
		FFrameTime OutputEvalPositionFrom = ConvertFrameTime(InputEvalPositionFrom, InputRate, OutputRate);
		FFrameTime OutputEvalPositionTo = ConvertFrameTime(NewPosition, InputRate, OutputRate);

		LastRange = FActActionEvaluationRange(OutputEvalPositionTo, OutputEvalPositionFrom, OutputRate, !PreviousPlayEvalPosition.IsSet());

		// Assign the cached input values
		CurrentPosition = NewPosition;
		PreviousPlayEvalPosition = NewPosition;

		return LastRange.GetValue();
	}

	FActActionEvaluationRange FActActionPlaybackPosition::GetCurrentPositionAsRange() const
	{
		CheckInvariants();

		FFrameTime OutputPosition = ConvertFrameTime(CurrentPosition, InputRate, OutputRate);
		return FActActionEvaluationRange(OutputPosition, OutputRate);
	}

	void FActActionPlaybackPosition::CheckInvariants() const
	{
		checkf(InputRate.IsValid() && OutputRate.IsValid(), TEXT("Invalid input or output rate. SetTimeBase must be called before any use of this class."))
	}

	FActImageTrackCarNotifyNode::FActImageTrackCarNotifyNode(FAnimNotifyEvent* InAnimNotifyEvent)
		: NotifyEvent(InAnimNotifyEvent),
		  Guid(NotifyEvent->Guid) {}

	TOptional<FLinearColor> FActImageTrackCarNotifyNode::GetEditorColor()
	{
		TOptional<FLinearColor> ReturnColour;
		if (NotifyEvent->Notify)
		{
			ReturnColour = NotifyEvent->Notify->GetEditorColor();
		}
		else if (NotifyEvent->NotifyStateClass)
		{
			ReturnColour = NotifyEvent->NotifyStateClass->GetEditorColor();
		}
		return ReturnColour;
	}

	FText FActImageTrackCarNotifyNode::GetNodeTooltip(const UAnimSequenceBase* Sequence)
	{
		FText ToolTipText = MakeTooltipFromTime(Sequence, NotifyEvent->GetTime(), NotifyEvent->GetDuration());

		if (NotifyEvent->IsBranchingPoint())
		{
			ToolTipText = FText::Format(LOCTEXT("AnimNotify_ToolTipBranchingPoint", "{0} (BranchingPoint)"), ToolTipText);
		}

		UObject* NotifyToDisplayClassOf = NotifyEvent->Notify;
		if (NotifyToDisplayClassOf == nullptr)
		{
			NotifyToDisplayClassOf = NotifyEvent->NotifyStateClass;
		}

		if (NotifyToDisplayClassOf != nullptr)
		{
			ToolTipText = FText::Format(LOCTEXT("AnimNotify_ToolTipNotifyClass", "{0}\nClass: {1}"), ToolTipText, NotifyToDisplayClassOf->GetClass()->GetDisplayNameText());
		}

		return ToolTipText;
	}

	TOptional<UObject*> FActImageTrackCarNotifyNode::GetObjectBeingDisplayed()
	{
		if (NotifyEvent->Notify)
		{
			return TOptional<UObject*>(NotifyEvent->Notify);
		}

		if (NotifyEvent->NotifyStateClass)
		{
			return TOptional<UObject*>(NotifyEvent->NotifyStateClass);
		}
		return TOptional<UObject*>();
	}

	void FActImageTrackCarNotifyNode::HandleDrop(UAnimSequenceBase* Sequence, float Time, int32 TrackIndex)
	{
		float EventDuration = NotifyEvent->GetDuration();

		NotifyEvent->Link(Sequence, Time, NotifyEvent->GetSlotIndex());
		NotifyEvent->RefreshTriggerOffset(Sequence->CalculateOffsetForNotify(NotifyEvent->GetTime()));

		if (EventDuration > 0.0f)
		{
			NotifyEvent->EndLink.Link(Sequence, NotifyEvent->GetTime() + EventDuration, NotifyEvent->GetSlotIndex());
			NotifyEvent->RefreshEndTriggerOffset(Sequence->CalculateOffsetForNotify(NotifyEvent->EndLink.GetTime()));
		}
		else
		{
			NotifyEvent->EndTriggerTimeOffset = 0.0f;
		}

		NotifyEvent->TrackIndex = TrackIndex;
	}

	void FActImageTrackCarNotifyNode::CacheName()
	{
		if (NotifyEvent->Notify)
		{
			CachedNotifyName = FName(*NotifyEvent->Notify->GetNotifyName());
		}
		else if (NotifyEvent->NotifyStateClass)
		{
			CachedNotifyName = FName(*NotifyEvent->NotifyStateClass->GetNotifyName());
		}
		else
		{
			CachedNotifyName = NotifyEvent->NotifyName;
		}
	}

	void FActImageTrackCarNotifyNode::Delete(UAnimSequenceBase* Seq)
	{
		for (int32 I = 0; I < Seq->Notifies.Num(); ++I)
		{
			if (NotifyEvent == &(Seq->Notifies[I]))
			{
				Seq->Notifies.RemoveAt(I);
				Seq->PostEditChange();
				Seq->MarkPackageDirty();
				break;
			}
		}
	}

	void FActImageTrackCarNotifyNode::MarkForDelete(UAnimSequenceBase* Seq)
	{
		for (int32 I = 0; I < Seq->Notifies.Num(); ++I)
		{
			if (NotifyEvent == &(Seq->Notifies[I]))
			{
				Seq->Notifies[I].Guid = FGuid();
				break;
			}
		}
	}

	void FActImageTrackCarNotifyNode::ExportForCopy(UAnimSequenceBase* Seq, FString& StrValue) const
	{
		int32 Index = INDEX_NONE;
		for (int32 NotifyIdx = 0; NotifyIdx < Seq->Notifies.Num(); ++NotifyIdx)
		{
			if (NotifyEvent == &Seq->Notifies[NotifyIdx])
			{
				Index = NotifyIdx;
				break;
			}
		}

		check(Index != INDEX_NONE);

		FArrayProperty* ArrayProperty = NULL;
		uint8* PropertyData = Seq->FindNotifyPropertyData(Index, ArrayProperty);
		if (PropertyData && ArrayProperty)
		{
			ArrayProperty->Inner->ExportTextItem(StrValue, PropertyData, PropertyData, Seq, PPF_Copy);
		}
	}

	FText FActImageTrackCarNotifyNode::MakeTooltipFromTime(const UAnimSequenceBase* InSequence, float InSeconds, float InDuration)
	{
		const FText Frame = FText::AsNumber(InSequence->GetFrameAtTime(InSeconds));
		const FText Seconds = FText::AsNumber(InSeconds);

		if (InDuration > 0.0f)
		{
			const FText Duration = FText::AsNumber(InDuration);
			return FText::Format(LOCTEXT("NodeToolTipLong", "@ {0} sec (frame {1}) for {2} sec"), Seconds, Frame, Duration);
		}
		else
		{
			return FText::Format(LOCTEXT("NodeToolTipShort", "@ {0} sec (frame {1})"), Seconds, Frame);
		}
	}

	void FActImageTrackCarNotifyNode::RemoveInvalidNotifies(UAnimSequenceBase* SeqBase)
	{
		SeqBase->Notifies.RemoveAll([](const FAnimNotifyEvent& InNotifyEvent) { return !InNotifyEvent.Guid.IsValid(); });
		SeqBase->PostEditChange();
		SeqBase->MarkPackageDirty();
	}

	FActImageTrackPadding::FActImageTrackPadding(float InUniform)
		: Top(InUniform), Bottom(InUniform) {}

	FActImageTrackPadding::FActImageTrackPadding(float InTop, float InBottom)
		: Top(InTop), Bottom(InBottom) {}

	float FActImageTrackPadding::Combined() const
	{
		return Top + Bottom;
	}
}

#undef LOCTEXT_NAMESPACE
