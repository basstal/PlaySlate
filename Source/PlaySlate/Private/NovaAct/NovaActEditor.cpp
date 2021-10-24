#include "NovaActEditor.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "NovaAct/Assets/ActAnimation.h"
#include "NovaAct/ActEventTimeline/ActEventTimeline.h"
#include "NovaAct/ActViewport/ActViewport.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActViewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/ActActionSequenceWidget.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"
#include "NovaAct/ActAssetDetails/ActAssetDetails.h"

#include "Animation/AnimBlueprint.h"
#include "Common/NovaDataBinding.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaStruct;

FNovaActEditor::FNovaActEditor(UActAnimation* InActAnimation)
{
	check(InActAnimation);
	ActAnimationDB = NovaDB::CreateUObject("ActAnimation", InActAnimation);
}

FNovaActEditor::~FNovaActEditor()
{
	UE_LOG(LogActAction, Log, TEXT("FNovaActEditor::~FNovaActEditor"));
	ActViewport.Reset();
	ActEventTimeline.Reset();
	NovaDB::Delete<UActAnimation*>("ActAnimation");
	ActAnimationDB.Reset();
}

void FNovaActEditor::CreateEditorWindow(const TSharedPtr<IToolkitHost>& InIToolkitHost)
{
	/**
	 * Editor（大页签）内的每个窗口由一个个Tab组成，这里载入这些Tab和对应的Layout信息，
	 * 注意：AddTab必须是已经注册过的TabId，注册的过程是通过重载RegisterTabSpawners
	 * 这个默认的Layout只在恢复默认Layout时有效，其他时候都是读取Layout在ini中的缓存信息，即加载之前保存的Layout
	 */
	const static TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_ActActionSequenceEditor");
	StandaloneDefaultLayout->AddArea(FTabManager::NewPrimaryArea()
	                                 ->SetOrientation(Orient_Horizontal)
	                                 ->Split(
		                                 FTabManager::NewStack()
		                                 ->AddTab(NovaConst::ActAssetDetailsTabId, ETabState::OpenedTab)
		                                 ->SetSizeCoefficient(0.3f))
	                                 ->Split(FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
	                                                                   ->Split(
		                                                                   FTabManager::NewStack()
		                                                                   ->AddTab(NovaConst::ActViewportTabId, ETabState::OpenedTab)
		                                                                   ->SetHideTabWell(true))
	                                                                   ->Split(
		                                                                   FTabManager::NewStack()
		                                                                   ->AddTab(NovaConst::ActEventTimelineTabId, ETabState::OpenedTab)
		                                                                   ->SetHideTabWell(true))));

	// Initialize the asset editor
	InitAssetEditor(EToolkitMode::Standalone, InIToolkitHost, NovaConst::NovaActAppName, StandaloneDefaultLayout, true, true, ActAnimationDB->GetData());

	// ** 通过对应Widget的Controller，填充Tab的实际内容Widget，并保留对Controller的引用
	// ** ActActionSequenceController
	ActEventTimeline = MakeShareable(new FActEventTimeline(SharedThis(this)));
	ActEventTimeline->Init();
	if (ActEventTimelineWidgetParent)
	{
		ActEventTimelineWidgetParent->SetContent(ActEventTimeline->GetActActionSequenceWidget());
	}

	const FPreviewScene::ConstructionValues CSV = FPreviewScene::ConstructionValues().AllowAudioPlayback(true).ShouldSimulatePhysics(true);
	ActViewport = MakeShareable(new FActViewport(CSV, SharedThis(this)));
	ActViewport->Init();
	if (ActViewportWidgetParent)
	{
		ActViewportWidgetParent->SetContent(ActViewport->GetActActionViewportWidget());
	}

	// ** DetailsView Controller
	ActAssetDetails = MakeShareable(new FActAssetDetails());
	ActAssetDetails->Init();
	if (ActAssetDetailsWidgetParent)
	{
		ActAssetDetailsWidgetParent->SetContent(ActAssetDetails->GetActActionDetailsViewWidget());
	}

	// ** Init by resource
	ActAnimationDB->Trigger();

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FNovaActEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	UActAnimation* Data = ActAnimationDB->GetData();
	Collector.AddReferencedObject(Data);
}

FString FNovaActEditor::GetReferencerName() const
{
	return "NovaActEditor";
}

FName FNovaActEditor::GetToolkitFName() const
{
	return FName("NovaActEditor");
}

FText FNovaActEditor::GetBaseToolkitName() const
{
	return LOCTEXT("ActEditor", "BaseToolkitName");
}

void FNovaActEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// ** NOTE:记得反注册中也要添加对应方法
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	FTabSpawnerEntry& ViewportEntry = InTabManager->RegisterTabSpawner(NovaConst::ActViewportTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActViewportTabSpawn));
	ViewportEntry.SetDisplayName(LOCTEXT("ActEditor", "ActViewport"));
	FTabSpawnerEntry& EventTimelineEntry = InTabManager->RegisterTabSpawner(NovaConst::ActEventTimelineTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActEventTimelineTabSpawn));
	EventTimelineEntry.SetDisplayName(LOCTEXT("ActEditor", "ActEventTimeline"));
	FTabSpawnerEntry& AssetDetailsEntry = InTabManager->RegisterTabSpawner(NovaConst::ActAssetDetailsTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActAssetDetailsTabSpawn));
	AssetDetailsEntry.SetDisplayName(LOCTEXT("ActEditor", "ActAssetDetails"));
}

void FNovaActEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(NovaConst::ActEventTimelineTabId);
	InTabManager->UnregisterTabSpawner(NovaConst::ActViewportTabId);
	InTabManager->UnregisterTabSpawner(NovaConst::ActAssetDetailsTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FLinearColor FNovaActEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

FString FNovaActEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("ActEditor", "WorldCentricTabPrefix").ToString();
}

TSharedRef<SDockTab> FNovaActEditor::OnActViewportTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActViewportWidgetParent = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActViewport"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
	return ActViewportWidgetParent.ToSharedRef();
}


TSharedRef<SDockTab> FNovaActEditor::OnActEventTimelineTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActEventTimelineWidgetParent = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActEventTimeline"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	return ActEventTimelineWidgetParent.ToSharedRef();
}

TSharedRef<SDockTab> FNovaActEditor::OnActAssetDetailsTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActAssetDetailsWidgetParent = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActAssetDetails"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	if (ActAssetDetails.IsValid())
	{
		ActAssetDetailsWidgetParent->SetContent(ActAssetDetails->GetActActionDetailsViewWidget());
	}
	return ActAssetDetailsWidgetParent.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
