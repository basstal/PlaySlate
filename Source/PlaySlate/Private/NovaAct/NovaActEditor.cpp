#include "NovaActEditor.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"

#include "NovaAct/ActViewport/ActViewportPreviewScene.h"
#include "NovaAct/ActEventTimeline/ActEventTimelineWidget.h"
#include "NovaAct/ActAssetDetails/ActAssetDetailsWidget.h"

#include "FrameNumberNumericInterface.h"
#include "ActViewport/ActViewport.h"

#define LOCTEXT_NAMESPACE "NovaAct"

using namespace NovaStruct;

FNovaActEditor::FNovaActEditor(UActAnimation* InActAnimation)
{
	check(InActAnimation);
	NovaDB::CreateUObject("ActAnimation", InActAnimation);
}

FNovaActEditor::~FNovaActEditor()
{
	UE_LOG(LogNovaAct, Log, TEXT("FNovaActEditor::~FNovaActEditor"));
	ActViewportPreviewScene.Reset();

	NovaDB::Delete("ActEventTimelineArgs");
	NovaDB::Delete("ActAnimation");
}

void FNovaActEditor::CreateEditorWindow(const TSharedPtr<IToolkitHost>& InIToolkitHost)
{
	/**
	 * Editor（大页签）内的每个窗口由一个个Tab组成，这里载入这些Tab和对应的Layout信息，
	 * 注意：AddTab必须是已经注册过的TabId，注册的过程是通过重载RegisterTabSpawners
	 * 这个默认的Layout只在恢复默认Layout时有效，其他时候都是读取Layout在ini中的缓存信息，即加载之前保存的Layout
	 */
	const static TSharedRef<FTabManager::FLayout> StandaloneDefaultLayout = FTabManager::NewLayout("Standalone_NovaActEditor");
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
	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	// Initialize the asset editor
	InitAssetEditor(EToolkitMode::Standalone,
	                InIToolkitHost,
	                NovaConst::NovaActAppName,
	                StandaloneDefaultLayout,
	                true,
	                true,
	                ActAnimationDB->GetData());

	// ** 填充Tab的实际内容Widget
	if (ActEventTimelineParentDockTab)
	{
		// ** EventTimeline 共享参数的初始化
		TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = MakeShareable(new FActEventTimelineArgs());
		auto TickResolutionAttrLambda = MakeAttributeLambda([]()
		{
			auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
			TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = DB->GetData();
			return ActEventTimelineArgs->TickResolution;
		});
		ActEventTimelineArgs->NumericTypeInterface = MakeShareable(new FFrameNumberInterface(MakeAttributeLambda([]()
		                                                                                     {
			                                                                                     return EFrameNumberDisplayFormats::Frames;
		                                                                                     }),
		                                                                                     0,
		                                                                                     TickResolutionAttrLambda,
		                                                                                     TickResolutionAttrLambda));
		NovaDB::CreateSP("ActEventTimelineArgs", ActEventTimelineArgs);

		FDelegateHandle _;
		// ** ActAnimation 数据绑定
		DataBindingUObjectBindRaw(UActAnimation, "ActAnimation", this, &FNovaActEditor::OnAnimSequenceChanged, _);

		// ** 构造 Widget 显示
		TSharedRef<SActEventTimelineWidget> ActEventTimelineWidget = SNew(SActEventTimelineWidget);
		ActEventTimelineParentDockTab->SetContent(ActEventTimelineWidget);
	}

	// ** Init Viewport Dock Tab
	const FPreviewScene::ConstructionValues CSV = FPreviewScene::ConstructionValues().AllowAudioPlayback(true).ShouldSimulatePhysics(true);
	ActViewportPreviewScene = MakeShareable(new FActViewportPreviewScene(CSV, SharedThis(this)));
	if (ActViewportParentDockTab)
	{
		ActViewportPreviewScene->Init(ActViewportParentDockTab.ToSharedRef());
	}

	// ** Init AssetDetails Dock Tab
	if (ActAssetDetailsParentDockTab)
	{
		TSharedRef<SWidget> ActAssetDetailsWidget = SNew(SActAssetDetailsWidget);
		ActAssetDetailsParentDockTab->SetContent(ActAssetDetailsWidget);
	}

	// ** Init by resource
	NovaDB::Trigger("ActEventTimelineArgs");

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FNovaActEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	UActAnimation* Data = DB->GetData();
	Collector.AddReferencedObject(Data);
}

FString FNovaActEditor::GetReferencerName() const
{
	return "NovaActEditor";
}

FName FNovaActEditor::GetToolkitFName() const
{
	return FName("ToolkitName");
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

void FNovaActEditor::Tick(float DeltaTime)
{
	// ** 使得 Client 能够保持 Tick，否则在失去焦点时 Client 将不会调用 Tick
	ActViewportPreviewScene->ActViewport->GetViewportClient()->Invalidate();
}

TStatId FNovaActEditor::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
}

TSharedRef<SDockTab> FNovaActEditor::OnActViewportTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActViewportParentDockTab = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActViewport"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
	return ActViewportParentDockTab.ToSharedRef();
}


TSharedRef<SDockTab> FNovaActEditor::OnActEventTimelineTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActEventTimelineParentDockTab = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActEventTimeline"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	return ActEventTimelineParentDockTab.ToSharedRef();
}

TSharedRef<SDockTab> FNovaActEditor::OnActAssetDetailsTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActAssetDetailsParentDockTab = SNew(SDockTab)
		.Label(LOCTEXT("ActEditor", "ActAssetDetails"))
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	return ActAssetDetailsParentDockTab.ToSharedRef();
}


void FNovaActEditor::OnAnimSequenceChanged(UActAnimation* InActAnimation)
{
	if (!InActAnimation)
	{
		return;
	}
	UAnimSequence* InAnimSequence = InActAnimation->AnimSequence;
	if (!InAnimSequence)
	{
		UE_LOG(LogNovaAct, Log, TEXT("FActEventTimeline::AddAnimMontageTrack with nullptr AnimMontage"))
		return;
	}
	UE_LOG(LogNovaAct, Log, TEXT("AnimMontage : %s"), *InAnimSequence->GetName());
	const float CalculateSequenceLength = InAnimSequence->GetPlayLength();
	UE_LOG(LogNovaAct, Log, TEXT("InTotalLength : %f"), CalculateSequenceLength);
	// ** 限制显示的最大长度为当前的Sequence总时长
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	auto ActEventTimelineArgs = DB->GetData();
	ActEventTimelineArgs->ViewRange->SetLowerBoundValue(0);
	ActEventTimelineArgs->ViewRange->SetUpperBoundValue(CalculateSequenceLength);
	ActEventTimelineArgs->ClampRange = TRange<float>(0, CalculateSequenceLength);
	ActEventTimelineArgs->TickResolution = InAnimSequence->GetSamplingFrameRate();;
}

#undef LOCTEXT_NAMESPACE
