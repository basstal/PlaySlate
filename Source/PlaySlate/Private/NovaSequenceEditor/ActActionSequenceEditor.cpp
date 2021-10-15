#include "ActActionSequenceEditor.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/Controllers/Sequence/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewSceneController.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaSequenceEditor/Widgets/Sequence/ActActionSequenceWidget.h"
#include "NovaSequenceEditor/Widgets/DetailsView/ActActionDetailsViewWidget.h"
#include "NovaSequenceEditor/Controllers/DetailsView/ActActionDetailsViewController.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/SkeletalMeshActor.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

FActActionSequenceEditor::FActActionSequenceEditor(UActActionSequence* InActActionSequence)
	: ActActionSequence(InActActionSequence),
	  PlaybackRange(0, 0),
	  SelectionRange(0, 0)
{
	check(InActActionSequence);
}

FActActionSequenceEditor::~FActActionSequenceEditor()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionSequenceEditor::~FActActionSequenceEditor"));
	ActActionPreviewSceneController.Reset();
	ActActionSequenceController.Reset();
}

void FActActionSequenceEditor::InitActActionSequenceEditor(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	/**
	 * Editor（大页签）内的每个窗口由一个个Tab组成，这里载入这些Tab和对应的Layout信息，
	 * 注意：AddTab必须是已经注册过的TabId，注册的过程是通过重载RegisterTabSpawners
	 * 这个默认的Layout只在恢复默认Layout时有效，其他时候都是读取Layout在ini中的缓存信息，即加载之前保存的Layout
	 */
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ActActionSequenceEditor");
	StandaloneDefaultLayout->AddArea(
		FTabManager::NewPrimaryArea()
		->SetOrientation(Orient_Vertical)
		->Split(
			FTabManager::NewStack()
			->AddTab(ActActionSequence::ActActionViewportTabId, ETabState::OpenedTab)
			->SetHideTabWell(true))
		->Split(
			FTabManager::NewStack()
			->AddTab(ActActionSequence::ActActionSequenceTabId, ETabState::OpenedTab)
			->SetHideTabWell(true))
	);
	// Initialize the asset editor
	InitAssetEditor(EToolkitMode::Standalone, InitToolkitHost, FName("ActAction_AppIdentifier"), StandaloneDefaultLayout, true, true, ActActionSequence);

	// ** 通过对应Widget的Controller，填充Tab的实际内容Widget，并保留对Controller的引用
	// ** ActActionSequenceController
	ActActionSequenceController = MakeShareable(new FActActionSequenceController(SharedThis(this)));
	ActActionSequenceController->ExecuteTrackEditorCreateDelegate();
	ActActionSequence::FActActionSequenceViewParams ViewParams = ActActionSequence::FActActionSequenceViewParams();
	ActActionSequenceController->MakeSequenceWidget(ViewParams);
	ActActionSequenceWidgetParent->SetContent(ActActionSequenceController->GetActActionSequenceWidget());

	// ** PreviewScene(Viewport)Controller
	FPreviewScene::ConstructionValues ConstructionValues = FPreviewScene::ConstructionValues()
	                                                       .AllowAudioPlayback(true)
	                                                       .ShouldSimulatePhysics(true);
	ActActionPreviewSceneController = MakeShareable(new FActActionPreviewSceneController(ConstructionValues, SharedThis(this)));
	ActActionPreviewSceneController->MakeViewportWidget();
	ActActionViewportWidgetParent->SetContent(ActActionPreviewSceneController->GetActActionViewportWidget());

	// ** DetailsView Controller
	ActActionDetailsViewController = MakeShareable(new FActActionDetailsViewController(SharedThis(this)));
	ActActionDetailsViewController->MakeDetailsViewWidget();
	if (ActActionDetailsViewWidgetParent)
	{
		ActActionDetailsViewWidgetParent->SetContent(ActActionDetailsViewController->GetActActionDetailsViewWidget());
	}


	// ** Init by resource
	InitAnimBlueprint(ActActionSequence->EditAnimBlueprint);
	ActActionSequenceController->AddAnimMontageTrack(ActActionSequence->EditAnimMontage);

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FActActionSequenceEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ActActionSequence);
}

FString FActActionSequenceEditor::GetReferencerName() const
{
	return "ActActionSequenceEditor";
}

FName FActActionSequenceEditor::GetToolkitFName() const
{
	return FName("ActActionSequenceEditor");
}

FText FActActionSequenceEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ActAction Sequence Editor");
}

void FActActionSequenceEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	FTabSpawnerEntry& TabSpawnerEntryViewport = InTabManager->RegisterTabSpawner(
		ActActionSequence::ActActionViewportTabId,
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
		{
			ActActionViewportWidgetParent = SNew(SDockTab)
				.Label(LOCTEXT("ActActionSequenceMainTitle", "ActActionViewport"))
				.TabColorScale(GetTabColorScale())
				.TabRole(ETabRole::PanelTab)
				.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
			return ActActionViewportWidgetParent.ToSharedRef();
		}));
	TabSpawnerEntryViewport.SetDisplayName(LOCTEXT("SequencerMainTab", "Viewport"));

	FTabSpawnerEntry& TabSpawnerEntrySequence = InTabManager->RegisterTabSpawner(
		ActActionSequence::ActActionSequenceTabId,
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
		{
			ActActionSequenceWidgetParent = SNew(SDockTab)
				.Label(LOCTEXT("ActActionSequenceMainTitle", "ActActionSequence"))
				.TabColorScale(GetTabColorScale())
				.TabRole(ETabRole::PanelTab)
				.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
			return ActActionSequenceWidgetParent.ToSharedRef();
		}));
	TabSpawnerEntrySequence.SetDisplayName(LOCTEXT("SequencerMainTab", "Sequence"));

	FTabSpawnerEntry& TabSpawnerEntryDetailsView = InTabManager->RegisterTabSpawner(
		ActActionSequence::ActActionDetailsViewTabId,
		FOnSpawnTab::CreateLambda([this](const FSpawnTabArgs& Args)-> TSharedRef<SDockTab>
		{
			ActActionDetailsViewWidgetParent = SNew(SDockTab)
				.Label(LOCTEXT("ActActionSequenceMainTitle", "ActActionDetailsView"))
				.TabColorScale(GetTabColorScale())
				.TabRole(ETabRole::PanelTab)
				.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
			if (ActActionDetailsViewController.IsValid())
			{
				ActActionDetailsViewWidgetParent->SetContent(ActActionDetailsViewController->GetActActionDetailsViewWidget());
			}
			return ActActionDetailsViewWidgetParent.ToSharedRef();
		}));
	TabSpawnerEntryDetailsView.SetDisplayName(LOCTEXT("SequencerMainTab", "DetailsView"));
}

void FActActionSequenceEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(ActActionSequence::ActActionSequenceTabId);
	InTabManager->UnregisterTabSpawner(ActActionSequence::ActActionViewportTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FLinearColor FActActionSequenceEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

FString FActActionSequenceEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "Sequencer ").ToString();
}

void FActActionSequenceEditor::InitAnimBlueprint(UAnimBlueprint* AnimBlueprint)
{
	if (!AnimBlueprint || !AnimBlueprint->TargetSkeleton)
	{
		UE_LOG(LogActAction, Error, TEXT("FActActionSequenceController::InitAnimBlueprint with nullptr AnimBlueprint or AnimBlueprint->TargetSkeleton is nullptr"));
		return;
	}

	if (ActActionSequence)
	{
		if (ActActionSequence->EditAnimBlueprint != AnimBlueprint)
		{
			UE_LOG(LogActAction, Log, TEXT("AssignAsEditAnim PreviewActor : %s"), *AnimBlueprint->GetName());
			ActActionSequence->EditAnimBlueprint = AnimBlueprint;
		}
		ActActionPreviewSceneController->SpawnActorInViewport(ASkeletalMeshActor::StaticClass(), AnimBlueprint);
	}
}

FFrameRate FActActionSequenceEditor::GetTickResolution() const
{
	return ActActionSequence->TickResolution;
}

FFrameRate FActActionSequenceEditor::GetDisplayRate() const
{
	return ActActionSequence->DisplayRate;
}


#undef LOCTEXT_NAMESPACE
