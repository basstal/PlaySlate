#include "ActTrackPanel.h"

#include "Common/NovaDataBinding.h"
#include "Common/NovaStruct.h"
#include "NovaAct/ActEventTimeline/SequenceNodeTree/ActActionTrackAreaSlot.h"

using namespace NovaStruct;

void SActTrackPanel::MakeTrackSlot()
{
	ActActionTrackAreaArgs.ViewInputMin.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
	{
		auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
		return ActEventTimelineArgs->ViewRange.GetLowerBoundValue();
	}));
	ActActionTrackAreaArgs.ViewInputMax.Bind(TAttribute<float>::FGetter::CreateLambda([this]()
	{
		auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
		return ActEventTimelineArgs->ViewRange.GetUpperBoundValue();
	}));
	auto TickResolutionLambda = TAttribute<FFrameRate>::FGetter::CreateLambda([this]()
	{
		auto ActEventTimelineArgsDB = NovaDB::GetOrCreate<TSharedPtr<FActEventTimelineArgs>>("ActEventTimelineArgs");
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = ActEventTimelineArgsDB->GetData();
		return ActEventTimelineArgs->TickResolution;
	});
	ActActionTrackAreaArgs.TickResolution.Bind(TickResolutionLambda);
	// ActActionSectionWidget = SNew(SActActionSequenceCombinedKeysTrack, SharedThis(this))
	// 	.Visibility(EVisibility::Visible)
	// 	.TickResolution(ActActionTrackAreaArgs.TickResolution);
	ActActionTrackAreaSlot = MakeShareable(new FActActionTrackAreaSlot(SharedThis(this)));
	ActActionTrackAreaSlot->MakeTrackLane();
}
