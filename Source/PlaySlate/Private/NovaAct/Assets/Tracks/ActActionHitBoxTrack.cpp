#include "ActActionHitBoxTrack.h"

#include "NovaAct/NovaActEditor.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
#include "NovaAct/Assets/ActAnimation.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActActionHitBoxTrack::FActActionHitBoxTrack(const TSharedRef<FActEventTimeline>& ActActionSequenceController)
	: FActActionTrackEditorBase(ActActionSequenceController)
{}

void FActActionHitBoxTrack::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AssignHitBox", "Assign HitBox"),
		LOCTEXT("AssignHitBox", "Assign HitBox"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
		FUIAction(FExecuteAction::CreateRaw(this, &FActActionHitBoxTrack::AssignHitBox))
	);
}

TSharedRef<FActActionTrackEditorBase> FActActionHitBoxTrack::CreateTrackEditor(TSharedRef<FActEventTimeline> InSequenceController)
{
	return MakeShareable(new FActActionHitBoxTrack(InSequenceController));
}

void FActActionHitBoxTrack::AssignHitBox() const
{
	auto DB = NovaDB::GetOrCreate<UActAnimation*>("ActAnimation");
	UActAnimation* ActAnimation = DB->GetData();
	ActAnimation->ActActionHitBoxes.AddDefaulted();
	DB->Trigger();
}

#undef LOCTEXT_NAMESPACE
