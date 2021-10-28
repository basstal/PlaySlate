#include "NovaActEditor.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "Common/NovaDataBinding.h"
#include "NovaAct/Assets/ActAnimation.h"

#include "NovaAct/ActViewport/ActViewportPreviewScene.h"
#include "NovaAct/ActEventTimeline/ActEventTimelineWidget.h"
#include "NovaAct/ActAssetDetails/ActAssetDetailsWidget.h"

#include "FrameNumberNumericInterface.h"
#include "NovaActEditorMode.h"
#include "ActViewport/ActViewport.h"
#include "Common/NovaStaticFunction.h"

#define LOCTEXT_NAMESPACE "NovaAct"

static const FText NovaActEditor_ActViewport = LOCTEXT("ActViewport", "NovaActEditor ActViewport tab name.");
static const FText NovaActEditor_ActEventTimeline = LOCTEXT("ActEventTimeline", "NovaActEditor ActEventTimeline tab name.");
static const FText NovaActEditor_ActAssetDetails = LOCTEXT("ActAssetDetails", "NovaActEditor ActAssetDetails tab name.");

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
	NovaDB::Delete("ActViewportPreviewScene");
	NovaDB::Delete("AnimSequence");
}

void FNovaActEditor::CreateEditorWindow(const TSharedPtr<IToolkitHost>& InIToolkitHost)
{
	FDelegateHandle _;

	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	NovaDB::CreateUObject("ActAnimation/AnimSequence", ActAnimationDB->GetData()->AnimSequence);
	DataBindingUObjectBindRaw(UAnimSequence, "ActAnimation/AnimSequence", this, &FNovaActEditor::OnAnimSequenceChanged, _);
	DataBindingUObjectBindRaw(UAnimationAsset, "ActAnimation/AnimSequence", this, &FNovaActEditor::OpenNewAnimationAssetEditTab, _)

	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = NovaStaticFunction::MakeActEventTimelineArgs();
	NovaDB::CreateSP("ActEventTimelineArgs", ActEventTimelineArgs);

	const FPreviewScene::ConstructionValues CSV = FPreviewScene::ConstructionValues().AllowAudioPlayback(true).ShouldSimulatePhysics(true);
	ActViewportPreviewScene = MakeShareable(new FActViewportPreviewScene(CSV));
	NovaDB::CreateSP("ActViewportPreviewScene", ActViewportPreviewScene);

	// Initialize the asset editor
	InitAssetEditor(EToolkitMode::Standalone,
	                InIToolkitHost,
	                NovaConst::NovaActAppName,
	                FTabManager::FLayout::NullLayout,
	                true,
	                true,
	                ActAnimationDB->GetData());

	AddApplicationMode(NovaConst::NovaActEditorMode, MakeShareable(new FNovaActEditorMode(SharedThis(this))));
	SetCurrentMode(NovaConst::NovaActEditorMode);


	// ** 填充Tab的实际内容Widget
	if (ActEventTimelineParentDockTab)
	{
		// ** 构造 Widget 显示
		TSharedRef<SActEventTimelineWidget> ActEventTimelineWidget = SNew(SActEventTimelineWidget);
		ActEventTimelineParentDockTab->SetContent(ActEventTimelineWidget);
	}


	// ** Init Viewport Dock Tab
	// if (ActViewportParentDockTab)
	// {
	// 	ActViewportPreviewScene->Init(ActViewportParentDockTab.ToSharedRef());
	// }

	// // ** Init AssetDetails Dock Tab
	// if (ActAssetDetailsParentDockTab)
	// {
	// 	TSharedRef<SWidget> ActAssetDetailsWidget = SNew(SActAssetDetailsWidget);
	// 	ActAssetDetailsParentDockTab->SetContent(ActAssetDetailsWidget);
	// }

	// ** Init by resource
	NovaDB::Trigger("ActEventTimelineArgs");
	NovaDB::Trigger("ActAnimation/AnimSequence");

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
	return "ToolkitName";
}

FText FNovaActEditor::GetBaseToolkitName() const
{
	return LOCTEXT("NovaActToolkitName", "TODO: name this");
}

void FNovaActEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_AnimationEditor", "Animation Editor"));

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	// // ** NOTE:记得反注册中也要添加对应方法
	// FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	//
	// FTabSpawnerEntry& ViewportEntry = InTabManager->RegisterTabSpawner(NovaConst::ActViewportTabIds[0], FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActViewportTabSpawn));
	// ViewportEntry.SetDisplayName(NovaActEditor_ActViewport);
	// FTabSpawnerEntry& EventTimelineEntry = InTabManager->RegisterTabSpawner(NovaConst::ActEventTimelineTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActEventTimelineTabSpawn));
	// EventTimelineEntry.SetDisplayName(NovaActEditor_ActEventTimeline);
	// FTabSpawnerEntry& AssetDetailsEntry = InTabManager->RegisterTabSpawner(NovaConst::ActAssetDetailsTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActAssetDetailsTabSpawn));
	// AssetDetailsEntry.SetDisplayName(NovaActEditor_ActAssetDetails);
}

void FNovaActEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// InTabManager->UnregisterTabSpawner(NovaConst::ActEventTimelineTabId);
	// InTabManager->UnregisterTabSpawner(NovaConst::ActViewportTabIds[0]);
	// InTabManager->UnregisterTabSpawner(NovaConst::ActAssetDetailsTabId);

	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
}

FLinearColor FNovaActEditor::GetWorldCentricTabColorScale() const
{
	return FLinearColor(0.7f, 0.0f, 0.0f, 0.5f);
}

FString FNovaActEditor::GetWorldCentricTabPrefix() const
{
	return LOCTEXT("WorldCentricTabPrefix", "world centric tab prefix.").ToString();
}

void FNovaActEditor::Tick(float DeltaTime)
{
	// ** 使得 Client 能够保持 Tick，否则在失去焦点时 Client 将不会调用 Tick
	if (ActViewportPreviewScene->ActViewport)
	{
		ActViewportPreviewScene->ActViewport->GetViewportClient()->Invalidate();
	}
}

TStatId FNovaActEditor::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ExampleAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
}

TSharedRef<SDockTab> FNovaActEditor::OnActViewportTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActViewportParentDockTab = SNew(SDockTab)
		.Label(NovaActEditor_ActViewport)
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab)
		.Icon(FEditorStyle::GetBrush("LevelEditor.Tabs.Viewports"));
	return ActViewportParentDockTab.ToSharedRef();
}


TSharedRef<SDockTab> FNovaActEditor::OnActEventTimelineTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActEventTimelineParentDockTab = SNew(SDockTab)
		.Label(NovaActEditor_ActEventTimeline)
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	return ActEventTimelineParentDockTab.ToSharedRef();
}

TSharedRef<SDockTab> FNovaActEditor::OnActAssetDetailsTabSpawn(const FSpawnTabArgs& SpawnTabArgs)
{
	ActAssetDetailsParentDockTab = SNew(SDockTab)
		.Label(NovaActEditor_ActAssetDetails)
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);
	return ActAssetDetailsParentDockTab.ToSharedRef();
}


void FNovaActEditor::OnAnimSequenceChanged(UAnimSequence* InAnimSequence)
{
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

void FNovaActEditor::OpenNewAnimationAssetEditTab(UAnimationAsset* InAnimationAsset)
{
	UE_LOG(LogNovaAct, Log, TEXT("FNovaActEditor::OpenNewAnimationAssetEditTab"));
	TSharedPtr<SDockTab> OpenedTab;

	if (InAnimationAsset != nullptr)
	{
		FString	DocumentLink;

		// FAnimDocumentArgs Args(PersonaToolkit->GetPreviewScene(), GetPersonaToolkit(), GetSkeletonTree()->GetEditableSkeleton(), OnSectionsChanged);
		// Args.OnDespatchObjectsSelected = FOnObjectsSelected::CreateSP(this, &FAnimationEditor::HandleObjectsSelected);
		// Args.OnDespatchInvokeTab = FOnInvokeTab::CreateSP(this, &FAssetEditorToolkit::InvokeTab);
		// Args.OnDespatchSectionsChanged = FSimpleDelegate::CreateSP(this, &FAnimationEditor::HandleSectionsChanged);
		//
		// FPersonaModule& PersonaModule = FModuleManager::GetModuleChecked<FPersonaModule>("Persona");
		// TSharedRef<SWidget> TabContents = PersonaModule.CreateEditorWidgetForAnimDocument(SharedThis(this), InAnimationAsset, Args, DocumentLink);
		//
		// if (AnimationAsset)
		// {
		// 	RemoveEditingObject(AnimationAsset);
		// }
		//
		// AddEditingObject(InAnimationAsset);
		// AnimationAsset = InAnimationAsset;
		//
		// GetPersonaToolkit()->GetPreviewScene()->SetPreviewAnimationAsset(InAnimationAsset);
		// GetPersonaToolkit()->SetAnimationAsset(InAnimationAsset);
		//
		// // Close existing opened curve tab
		// if(AnimCurveDocumentTab.IsValid())
		// {
		// 	AnimCurveDocumentTab.Pin()->RequestCloseTab();
		// }
		//
		// AnimCurveDocumentTab.Reset();
		//
		// struct Local
		// {
		// 	static FText GetObjectName(UObject* Object)
		// 	{
		// 		return FText::FromString(Object->GetName());
		// 	}
		// };
		//
		// TAttribute<FText> NameAttribute = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateStatic(&Local::GetObjectName, (UObject*)InAnimationAsset));
		//
		// const bool bIsReusedEditor = SharedAnimDocumentTab.IsValid();
		// if (bIsReusedEditor)
		// {
		// 	OpenedTab = SharedAnimDocumentTab.Pin();
		// 	OpenedTab->SetContent(TabContents);
		// 	OpenedTab->ActivateInParent(ETabActivationCause::SetDirectly);
		// 	OpenedTab->SetLabel(NameAttribute);
		// 	OpenedTab->SetLeftContent(IDocumentation::Get()->CreateAnchor(DocumentLink));
		// }
		// else
		// {
		// 	OpenedTab = SNew(SDockTab)
		// 		.Label(NameAttribute)
		// 		.TabRole(ETabRole::DocumentTab)
		// 		.TabColorScale(GetTabColorScale())
		// 		.OnTabClosed_Lambda([this](TSharedRef<SDockTab> InTab)
		// 		{
		// 			TSharedPtr<SDockTab> CurveTab = AnimCurveDocumentTab.Pin();
		// 			if(CurveTab.IsValid())
		// 			{
		// 				CurveTab->RequestCloseTab();
		// 			}
		// 		})
		// 		[
		// 			TabContents
		// 		];
		//
		// 	OpenedTab->SetLeftContent(IDocumentation::Get()->CreateAnchor(DocumentLink));
		//
		// 	TabManager->InsertNewDocumentTab(AnimationEditorTabs::DocumentTab, FTabManager::ESearchPreference::RequireClosedTab, OpenedTab.ToSharedRef());
		//
		// 	SharedAnimDocumentTab = OpenedTab;
		// }
		//
		// // Invoke the montage sections tab, and make sure the asset browser is there and in focus when we are dealing with a montage.
		// if(InAnimationAsset->IsA<UAnimMontage>())
		// {
		// 	TabManager->TryInvokeTab(AnimationEditorTabs::AnimMontageSectionsTab);
		//
		// 	// Only activate the asset browser tab when this is a reused Animation Editor window.
		// 	if (bIsReusedEditor)
		// 	{
		// 		TabManager->TryInvokeTab(AnimationEditorTabs::AssetBrowserTab);
		// 	}
		// 	OnSectionsChanged.Broadcast();
		// }
		// else
		// {
		// 	// Close existing opened montage sections tab
		// 	TSharedPtr<SDockTab> OpenMontageSectionsTab = TabManager->FindExistingLiveTab(AnimationEditorTabs::AnimMontageSectionsTab);
		// 	if(OpenMontageSectionsTab.IsValid())
		// 	{
		// 		OpenMontageSectionsTab->RequestCloseTab();
		// 	}	
		// }
		//
		// if (SequenceBrowser.IsValid())
		// {
		// 	SequenceBrowser.Pin()->SelectAsset(InAnimationAsset);
		// }
		//
		// // let the asset family know too
		// TSharedRef<IAssetFamily> AssetFamily = PersonaModule.CreatePersonaAssetFamily(InAnimationAsset);
		// AssetFamily->RecordAssetOpened(FAssetData(InAnimationAsset));
	}

	// return OpenedTab;
}

#undef LOCTEXT_NAMESPACE
