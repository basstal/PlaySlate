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
// : AnimationAsset(nullptr)
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
	NovaDB::Delete("ColumnFillCoefficientsLeft");
	NovaDB::Delete("ActAnimation/AnimSequence");
}

void FNovaActEditor::CreateEditorWindow(const TSharedPtr<IToolkitHost>& InIToolkitHost)
{
	FDelegateHandle _;

	auto ActAnimationDB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	NovaDB::Create("ActAnimation/AnimSequence", &ActAnimationDB->GetData()->AnimSequence);
	DataBindingBindRaw(UAnimSequence**, "ActAnimation/AnimSequence", this, &FNovaActEditor::OnAnimSequenceChanged, _);
	DataBindingBindRaw(UAnimationAsset**, "ActAnimation/AnimSequence", this, &FNovaActEditor::OpenNewAnimationAssetEditTab, _)

	TSharedPtr<FActEventTimelineArgs> ActEventTimelineArgs = NovaStaticFunction::MakeActEventTimelineArgs();
	NovaDB::CreateSP("ActEventTimelineArgs", ActEventTimelineArgs);

	const FPreviewScene::ConstructionValues CSV = FPreviewScene::ConstructionValues().AllowAudioPlayback(true).ShouldSimulatePhysics(true);
	ActViewportPreviewScene = MakeShareable(new FActViewportPreviewScene(CSV));
	NovaDB::CreateSP("ActViewportPreviewScene", ActViewportPreviewScene);

	// ** TODO:存储在配置中
	NovaDB::Create("ColumnFillCoefficientsLeft", 0.3f);

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
	// if (ActEventTimelineParentDockTab)
	// {
	// 	// ** 构造 Widget 显示
	// 	TSharedRef<SActEventTimelineWidget> ActEventTimelineWidget = SNew(SActEventTimelineWidget);
	// 	ActEventTimelineParentDockTab->SetContent(ActEventTimelineWidget);
	// }


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
	NovaDB::Trigger("ActAnimation");
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

	FTabSpawnerEntry& EventTimelineEntry = InTabManager->RegisterTabSpawner(NovaConst::ActEventTimelineTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActEventTimelineTabSpawn));
	EventTimelineEntry.SetMenuType(ETabSpawnerMenuType::Hidden);
	// EventTimelineEntry.SetDisplayName(NovaActEditor_ActEventTimeline);

	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	// // ** NOTE:记得反注册中也要添加对应方法
	// FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
	//
	// FTabSpawnerEntry& ViewportEntry = InTabManager->RegisterTabSpawner(NovaConst::ActViewportTabIds[0], FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActViewportTabSpawn));
	// ViewportEntry.SetDisplayName(NovaActEditor_ActViewport);
	// FTabSpawnerEntry& AssetDetailsEntry = InTabManager->RegisterTabSpawner(NovaConst::ActAssetDetailsTabId, FOnSpawnTab::CreateRaw(this, &FNovaActEditor::OnActAssetDetailsTabSpawn));
	// AssetDetailsEntry.SetDisplayName(NovaActEditor_ActAssetDetails);
}

void FNovaActEditor::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	InTabManager->UnregisterTabSpawner(NovaConst::ActEventTimelineTabId);
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
	// TSharedRef<SWidget> TabContents = SNullWidget::NullWidget;
	// auto DB = GetDataBindingUObject(UActAnimation, "ActAnimation");
	// if (DB)
	// {
	// 	TabContents = MakeEditTabContent(DB->GetData()->AnimSequence);
	// }

	ActEventTimelineParentDockTab = SNew(SDockTab)
		.Label(NovaActEditor_ActEventTimeline)
		.TabColorScale(GetTabColorScale())
		.TabRole(ETabRole::PanelTab);


	// [
	// 	TabContents
	// ];

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


void FNovaActEditor::OnAnimSequenceChanged(UAnimSequence** InAnimSequence)
{
	if (!InAnimSequence)
	{
		UE_LOG(LogNovaAct, Log, TEXT("FActEventTimeline::AddAnimMontageTrack with nullptr AnimMontage"))
		return;
	}
	UAnimSequence* AnimSequence = *InAnimSequence;
	UE_LOG(LogNovaAct, Log, TEXT("AnimMontage : %s"), *AnimSequence->GetName());
	const float CalculateSequenceLength = AnimSequence->GetPlayLength();
	UE_LOG(LogNovaAct, Log, TEXT("InTotalLength : %f"), CalculateSequenceLength);
	// ** 限制显示的最大长度为当前的Sequence总时长
	auto DB = GetDataBindingSP(FActEventTimelineArgs, "ActEventTimelineArgs");
	auto ActEventTimelineArgs = DB->GetData();
	ActEventTimelineArgs->ClampRange = TRange<double>(0, CalculateSequenceLength);
	ActEventTimelineArgs->SetViewRangeClamped(0, CalculateSequenceLength);
	NovaDB::Trigger("ActEventTimelineArgs/ViewRange");
	ActEventTimelineArgs->TickResolution = AnimSequence->GetSamplingFrameRate();
	NovaDB::Trigger("ActEventTimelineArgs");
}

TSharedRef<SWidget> FNovaActEditor::MakeEditTabContent(UAnimationAsset* InAnimationAsset)
{
	TSharedPtr<SWidget> Result = SNullWidget::NullWidget;
	if (InAnimationAsset)
	{
		// TWeakPtr<IAnimationEditor> WeakHostingApp = InHostingApp;
		// auto OnEditCurves = [WeakHostingApp](UAnimSequenceBase* InAnimSequence, const TArray<IAnimationEditor::FCurveEditInfo>& InCurveInfo, const TSharedPtr<ITimeSliderController>& InExternalTimeSliderController)
		// { 
		// 	WeakHostingApp.Pin()->EditCurves(InAnimSequence, InCurveInfo, InExternalTimeSliderController);
		// };

		// auto OnStopEditingCurves = [WeakHostingApp](const TArray<IAnimationEditor::FCurveEditInfo>& InCurveInfo)
		// { 
		// 	WeakHostingApp.Pin()->StopEditingCurves(InCurveInfo);
		// };

		if (Cast<UAnimSequence>(InAnimationAsset))
		{
			Result = SNew(SActEventTimelineWidget);
			// .Sequence(Sequence)
			// .OnObjectsSelected(InArgs.OnDespatchObjectsSelected)
			// .OnInvokeTab(InArgs.OnDespatchInvokeTab)
			// .OnEditCurves_Lambda(OnEditCurves)
			// .OnStopEditingCurves_Lambda(OnStopEditingCurves);

			// OutDocumentLink = TEXT("Engine/Animation/Sequences");
		}
		// ** TODO: 其他的 AnimationAsset
		// else if (UAnimComposite* Composite = Cast<UAnimComposite>(InAnimationAsset))
		// {
		// 	Result = SNew(SAnimCompositeEditor, InArgs.PreviewScene.Pin().ToSharedRef(), InArgs.EditableSkeleton.Pin().ToSharedRef(), InHostingApp->GetToolkitCommands())
		// 		.Composite(Composite)
		// 		.OnObjectsSelected(InArgs.OnDespatchObjectsSelected)
		// 		.OnInvokeTab(InArgs.OnDespatchInvokeTab)
		// 		.OnEditCurves_Lambda(OnEditCurves)
		// 		.OnStopEditingCurves_Lambda(OnStopEditingCurves);
		//
		// 	OutDocumentLink = TEXT("Engine/Animation/AnimationComposite");
		// }
		// else if (UAnimMontage* Montage = Cast<UAnimMontage>(InAnimationAsset))
		// {
		// 	FMontageEditorRequiredArgs RequiredArgs(InArgs.PreviewScene.Pin().ToSharedRef(), InArgs.EditableSkeleton.Pin().ToSharedRef(), InArgs.OnSectionsChanged, InHostingApp->GetToolkitCommands());
		//
		// 	Result = SNew(SMontageEditor, RequiredArgs)
		// 		.Montage(Montage)
		// 		.OnSectionsChanged(InArgs.OnDespatchSectionsChanged)
		// 		.OnInvokeTab(InArgs.OnDespatchInvokeTab)
		// 		.OnObjectsSelected(InArgs.OnDespatchObjectsSelected)
		// 		.OnEditCurves_Lambda(OnEditCurves)
		// 		.OnStopEditingCurves_Lambda(OnStopEditingCurves);
		//
		// 	OutDocumentLink = TEXT("Engine/Animation/AnimMontage");
		// }
		// else if (UAnimStreamable* StreamableAnim = Cast<UAnimStreamable>(InAnimationAsset))
		// {
		// 	Result = SNew(SAnimStreamableEditor, InArgs.PreviewScene.Pin().ToSharedRef(), InArgs.EditableSkeleton.Pin().ToSharedRef(), InHostingApp->GetToolkitCommands())
		// 		.StreamableAnim(StreamableAnim)
		// 		.OnObjectsSelected(InArgs.OnDespatchObjectsSelected)
		// 		.OnInvokeTab(InArgs.OnDespatchInvokeTab)
		// 		.OnEditCurves_Lambda(OnEditCurves)
		// 		.OnStopEditingCurves_Lambda(OnStopEditingCurves);
		//
		// 	OutDocumentLink = TEXT("Engine/Animation/Sequences");
		// }
		// else if (UPoseAsset* PoseAsset = Cast<UPoseAsset>(InAnimationAsset))
		// {
		// 	Result = SNew(SPoseEditor, InArgs.PersonaToolkit.Pin().ToSharedRef(), InArgs.EditableSkeleton.Pin().ToSharedRef(), InArgs.PreviewScene.Pin().ToSharedRef())
		// 		.PoseAsset(PoseAsset);
		//
		// 	OutDocumentLink = TEXT("Engine/Animation/Sequences");
		// }
		// else if (UBlendSpace* BlendSpace = Cast<UBlendSpace>(InAnimationAsset))
		// {
		// 	Result = SNew(SBlendSpaceEditor, InArgs.PreviewScene.Pin().ToSharedRef())
		// 		.BlendSpace(BlendSpace);
		//
		// 	if (Cast<UAimOffsetBlendSpace>(InAnimationAsset))
		// 	{
		// 		OutDocumentLink = TEXT("Engine/Animation/AimOffset");
		// 	}
		// 	else
		// 	{
		// 		OutDocumentLink = TEXT("Engine/Animation/Blendspaces");
		// 	}
		// }
		// else if (UBlendSpace1D* BlendSpace1D = Cast<UBlendSpace1D>(InAnimationAsset))
		// {
		// 	Result = SNew(SBlendSpaceEditor1D, InArgs.PreviewScene.Pin().ToSharedRef())
		// 		.BlendSpace1D(BlendSpace1D);
		//
		// 	if (Cast<UAimOffsetBlendSpace1D>(InAnimationAsset))
		// 	{
		// 		OutDocumentLink = TEXT("Engine/Animation/AimOffset");
		// 	}
		// 	else
		// 	{
		// 		OutDocumentLink = TEXT("Engine/Animation/Blendspaces");
		// 	}
		// }
	}

	if (Result.IsValid())
	{
		InAnimationAsset->SetFlags(RF_Transactional);
	}

	return Result.ToSharedRef();
}

void FNovaActEditor::OpenNewAnimationAssetEditTab(UAnimationAsset** InAnimationAsset)
{
	UE_LOG(LogNovaAct, Log, TEXT("FNovaActEditor::OpenNewAnimationAssetEditTab"));


	if (InAnimationAsset != nullptr)
	{
		UAnimationAsset* AnimationAsset = *InAnimationAsset;
		// FString	DocumentLink;

		// FAnimDocumentArgs Args(PersonaToolkit->GetPreviewScene(), GetPersonaToolkit(), GetSkeletonTree()->GetEditableSkeleton(), OnSectionsChanged);
		// Args.OnDespatchObjectsSelected = FOnObjectsSelected::CreateSP(this, &FAnimationEditor::HandleObjectsSelected);
		// Args.OnDespatchInvokeTab = FOnInvokeTab::CreateSP(this, &FAssetEditorToolkit::InvokeTab);
		// Args.OnDespatchSectionsChanged = FSimpleDelegate::CreateSP(this, &FAnimationEditor::HandleSectionsChanged);

		// FPersonaModule& PersonaModule = FModuleManager::GetModuleChecked<FPersonaModule>("Persona");

		// if (AnimationAsset)
		// {
		// 	RemoveEditingObject(AnimationAsset);
		// }
		//
		// AddEditingObject(InAnimationAsset);
		// AnimationAsset = InAnimationAsset;

		// Close existing opened curve tab
		// if(AnimCurveDocumentTab.IsValid())
		// {
		// 	AnimCurveDocumentTab.Pin()->RequestCloseTab();
		// }
		//
		// AnimCurveDocumentTab.Reset();

		// struct Local
		// {
		// 	static FText GetObjectName(UObject* Object)
		// 	{
		// 		return FText::FromString(Object->GetName());
		// 	}
		// };

		// TAttribute<FText> NameAttribute = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateStatic(&Local::GetObjectName, (UObject*)InAnimationAsset));

		if (ActEventTimelineParentDockTab)
		{
			TSharedRef<SWidget> TabContents = MakeEditTabContent(AnimationAsset);
			ActEventTimelineParentDockTab->SetContent(TabContents);
			ActEventTimelineParentDockTab->ActivateInParent(ETabActivationCause::SetDirectly);
			// OpenedTab->SetLabel(NameAttribute);
			// OpenedTab->SetLeftContent(IDocumentation::Get()->CreateAnchor(DocumentLink));
		}
		else
		{
			// OpenedTab = 
			// .OnTabClosed_Lambda([this](TSharedRef<SDockTab> InTab)
			// {
			// 	TSharedPtr<SDockTab> CurveTab = AnimCurveDocumentTab.Pin();
			// 	if(CurveTab.IsValid())
			// 	{
			// 		CurveTab->RequestCloseTab();
			// 	}
			// })
			// [
			// 	TabContents
			// ];

			// OpenedTab->SetLeftContent(IDocumentation::Get()->CreateAnchor(DocumentLink));
			TabManager->TryInvokeTab(NovaConst::ActEventTimelineTabId);
			// SharedAnimationAssetTab = OpenedTab;
		}

		// Invoke the montage sections tab, and make sure the asset browser is there and in focus when we are dealing with a montage.
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

		// if (SequenceBrowser.IsValid())
		// {
		// 	SequenceBrowser.Pin()->SelectAsset(InAnimationAsset);
		// }
		//
		// // let the asset family know too
		// TSharedRef<IAssetFamily> AssetFamily = PersonaModule.CreatePersonaAssetFamily(InAnimationAsset);
		// AssetFamily->RecordAssetOpened(FAssetData(InAnimationAsset));
	}
}

#undef LOCTEXT_NAMESPACE
