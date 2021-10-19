#include "ActActionHitBoxTrack.h"

#include "NovaSequenceEditor/ActActionSequenceEditor.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"

#define LOCTEXT_NAMESPACE "ActAction"

FActActionHitBoxTrack::FActActionHitBoxTrack(const TSharedRef<FActActionSequenceController>& ActActionSequenceController)
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

TSharedRef<FActActionTrackEditorBase> FActActionHitBoxTrack::CreateTrackEditor(TSharedRef<FActActionSequenceController> InSequenceController)
{
	return MakeShareable(new FActActionHitBoxTrack(InSequenceController));
}

void FActActionHitBoxTrack::AssignHitBox() const
{
	ActActionSequenceController.Pin()->GetActActionSequenceEditor()->AddHitBox();
}

#undef LOCTEXT_NAMESPACE
