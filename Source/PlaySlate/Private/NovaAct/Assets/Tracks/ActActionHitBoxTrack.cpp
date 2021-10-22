﻿#include "ActActionHitBoxTrack.h"

#include "NovaAct/NovaActEditor.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineBrain.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActActionHitBoxTrack::FActActionHitBoxTrack(const TSharedRef<FActEventTimelineBrain>& ActActionSequenceController)
	: FActActionTrackEditorBase(ActActionSequenceController)
{
}

void FActActionHitBoxTrack::BuildAddTrackMenu(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.AddMenuEntry(
		LOCTEXT("AssignHitBox", "Assign HitBox"),
		LOCTEXT("AssignHitBox", "Assign HitBox"),
		FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetTreeFolderOpen"),
		FUIAction(FExecuteAction::CreateRaw(this, &FActActionHitBoxTrack::AssignHitBox))
	);
}

TSharedRef<FActActionTrackEditorBase> FActActionHitBoxTrack::CreateTrackEditor(TSharedRef<FActEventTimelineBrain> InSequenceController)
{
	return MakeShareable(new FActActionHitBoxTrack(InSequenceController));
}

void FActActionHitBoxTrack::AssignHitBox() const
{
	ActActionSequenceController.Pin()->GetActActionSequenceEditor()->AddHitBox();
}

#undef LOCTEXT_NAMESPACE
