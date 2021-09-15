#include "ActActionSequenceEditor.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

void FActActionSequenceEditor::InitActActionSequenceEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UActActionSequence* InActActionSequence)
{
	// create tab layout
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ActActionSequenceEditor")
		->AddArea(FTabManager::NewPrimaryArea()
			->Split(FTabManager::NewStack()
				->AddTab(FName("ActActionSequence_Main"), ETabState::OpenedTab))
		);

	ActActionSequence = InActActionSequence;

	// Initialize the asset editor
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = true;
	InitAssetEditor(Mode, InitToolkitHost, FName("ActActionSequence"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ActActionSequence);

	ActActionSequenceController = MakeShared<FActActionSequenceController>();
	// Make internal widgets
	SequenceMain = SNew(SActActionSequenceMain, ActActionSequenceController->AsShared());

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FActActionSequenceEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ActActionSequence);
}

FLinearColor FActActionSequenceEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

FName FActActionSequenceEditor::GetToolkitFName() const
{
	return FName("ActActionSequenceEditor");
}

FText FActActionSequenceEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ActAction Sequence Editor");
}

FString FActActionSequenceEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Sequencer ").ToString();
}

void FActActionSequenceEditor::Tick(float DeltaTime)
{
}

TStatId FActActionSequenceEditor::GetStatId() const
{
	// ** TODO:STATGROUP_Tickables临时的标记
	RETURN_QUICK_DECLARE_CYCLE_STAT(FActActionSequenceEditor, STATGROUP_Tickables);
}

#undef LOCTEXT_NAMESPACE
