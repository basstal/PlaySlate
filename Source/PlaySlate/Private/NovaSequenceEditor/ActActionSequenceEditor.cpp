#include "ActActionSequenceEditor.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Assets/ActActionSequence.h"
#include "NovaSequenceEditor/Controllers/ActActionSequenceController.h"
#include "NovaSequenceEditor/Controllers/SequenceNodeTree/ActActionSequenceNodeTree.h"
#include "NovaSequenceEditor/Controllers/Viewport/ActActionPreviewScene.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "NovaSequenceEditor/Widgets/ActActionSequenceWidget.h"

#define LOCTEXT_NAMESPACE "ActActionToolkit"

FActActionSequenceEditor::~FActActionSequenceEditor()
{
	// ** Reset all widgets
	SequenceMain.Reset();
	Viewport.Reset();
	ActActionSequenceController.Reset();

	// FAssetEditorToolkit::~FAssetEditorToolkit();
	// FTickableEditorObject::~FTickableEditorObject();
	// FEditorUndoClient::~FEditorUndoClient();
	// FGCObject::~FGCObject();
}

void FActActionSequenceEditor::InitActActionSequenceEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UActActionSequence* InActActionSequence)
{
	// ** Editor内的每个窗口由一个个tab组成，这里载入这些tab和对应的layout信息，注意：AddTab必须是已经注册过的TabId，注册的过程
	// ** 这个默认的Layout只在恢复默认Layout时有效，其他时候都是读取Layout的ini缓存信息
	const TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ActActionSequenceEditor")
		->AddArea(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split(
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->AddTab(ActActionSequence::ActActionViewportTabId, ETabState::OpenedTab))
			->Split(
				FTabManager::NewStack()
				->SetHideTabWell(true)
				->AddTab(ActActionSequence::ActActionSequenceTabId, ETabState::OpenedTab))
		);

	// ViewportTabContent = MakeShareable(new FEditorViewportTabContent());

	ActActionSequencePtr = InActActionSequence;

	ActActionSequenceController = MakeShareable(new FActActionSequenceController(ActActionSequencePtr));
	TSharedPtr<FActActionSequenceNodeTree> ActActionSequenceNodeTree = MakeShareable(new FActActionSequenceNodeTree(ActActionSequenceController.ToSharedRef()));
	ActActionSequenceController->SetNodeTree(ActActionSequenceNodeTree);

	ActActionSequence::FActActionSequenceViewParams ViewParams = ActActionSequence::FActActionSequenceViewParams();
	// Make internal widgets
	SequenceMain = ActActionSequenceController->MakeSequenceWidget(ViewParams);

	// Initialize the asset editor
	const bool bCreateDefaultStandaloneMenu = true;
	const bool bCreateDefaultToolbar = false;
	InitAssetEditor(Mode, InitToolkitHost, FName("ActAction_AppIdentifier"), StandaloneDefaultLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, ActActionSequencePtr);

	TSharedPtr<FActActionPreviewScene> ActActionPreviewScene = MakeShareable(new FActActionPreviewScene(FPreviewScene::ConstructionValues().AllowAudioPlayback(true).ShouldSimulatePhysics(true)));
	ActActionPreviewScene->InitPreviewScene(nullptr);
	TSharedRef<FActActionSequenceEditor> ActActionSequenceEditorRef = StaticCastSharedRef<FActActionSequenceEditor>(AsShared());
	Viewport = SNew(SActActionViewportWidget, ActActionSequenceEditorRef, ActActionPreviewScene.ToSharedRef());
	FPlaySlateModule& PlaySlateModule = FModuleManager::Get().LoadModuleChecked<FPlaySlateModule>(TEXT("PlaySlate"));
	ActActionSequenceController->InitController(Viewport.ToSharedRef(), PlaySlateModule.GetTrackEditorDelegates(), SequenceMain.ToSharedRef());

	// ** 将Viewport填充到DockTab中
	if (ViewportParentDockTab.IsValid())
	{
		ViewportParentDockTab.Get()->SetContent(Viewport.ToSharedRef());
	}

	ActActionSequenceController->InitAnimBlueprint(ActActionSequencePtr->EditAnimBlueprint);
	ActActionSequenceController->AddAnimMontageTrack(ActActionSequencePtr->EditAnimMontage);
	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FActActionSequenceEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ActActionSequencePtr);
}

FString FActActionSequenceEditor::GetReferencerName() const
{
	return "ActActionSequenceEditor";
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

void FActActionSequenceEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	/**
	* For World Centric Asset Editors this isn't called until way too late in the initialization flow
	* (ie: when you actually start to edit an asset), so the tab will be unrecognized upon restore.
	* Because of this, the Sequencer Tab Spawner is actually registered in SLevelEditor.cpp manually
	* which is early enough that you can restore the tab after an editor restart.
	*/
	if (IsWorldCentricAssetEditor())
	{
		return;
	}
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_ActActionSequenceAssetEditor", "ActActionSequence"));

	InTabManager->RegisterTabSpawner(
		            ActActionSequence::ActActionSequenceTabId,
		            FOnSpawnTab::CreateSP(this, &FActActionSequenceEditor::HandleTabManagerSpawnSequence))
	            .SetDisplayName(LOCTEXT("SequencerMainTab", "Sequence"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());
	// .SetIcon(FSlateIcon(Style->GetStyleSetName(), "LevelSequenceEditor.Tabs.Sequencer"));

	InTabManager->RegisterTabSpawner(
		            ActActionSequence::ActActionViewportTabId,
		            FOnSpawnTab::CreateSP(this, &FActActionSequenceEditor::HandleTabManagerSpawnViewport))
	            .SetDisplayName(LOCTEXT("SequencerMainTab", "Viewport"))
	            .SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FActActionSequenceEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	if (!IsWorldCentricAssetEditor())
	{
		InTabManager->UnregisterTabSpawner(ActActionSequence::ActActionSequenceTabId);
		InTabManager->UnregisterTabSpawner(ActActionSequence::ActActionViewportTabId);
	}

	// ** TODO: remove when world-centric mode is added
	// FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	// LevelEditorModule.AttachSequencer(SNullWidget::NullWidget, nullptr);
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

// void FActActionSequenceEditor::CreateEditorModeManager()
// {
// 	FWorkflowCentricApplication::CreateEditorModeManager();
// }

TSharedRef<SDockTab> FActActionSequenceEditor::HandleTabManagerSpawnViewport(const FSpawnTabArgs& Args)
{
	// ** 先创建一个空的，后面再来填充内容
	ViewportParentDockTab = SNew(SDockTab)
		.Label(LOCTEXT("ActActionSequenceMainTitle", "ActActionViewport"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
	return ViewportParentDockTab.ToSharedRef();
}

TSharedRef<SDockTab> FActActionSequenceEditor::HandleTabManagerSpawnSequence(const FSpawnTabArgs& Args)
{
	TSharedPtr<SWidget> TabWidget = SNullWidget::NullWidget;

	if (SequenceMain)
	{
		if (Args.GetTabId() == ActActionSequence::ActActionSequenceTabId)
		{
			TabWidget = StaticCastSharedPtr<SActActionSequenceWidget>(SequenceMain);
		}
	}
	else
	{
		UE_LOG(LogActAction, Error, TEXT("SequenceMain is nullptr when HandleTabManagerSpawnSequence."));
	}

	return SNew(SDockTab)
		.Label(LOCTEXT("ActActionSequenceMainTitle", "ActActionSequence"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab)
	[
		TabWidget.ToSharedRef()
	];
}

#undef LOCTEXT_NAMESPACE
