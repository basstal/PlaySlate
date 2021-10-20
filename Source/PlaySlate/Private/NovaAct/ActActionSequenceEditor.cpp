#include "ActActionSequenceEditor.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "NovaAct/Assets/ActActionSequence.h"
#include "NovaAct/Controllers/ActEventTimeline/ActEventTimelineBrain.h"
#include "NovaAct/Controllers/ActViewport/ActActionPreviewSceneController.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActViewport/ActActionViewportWidget.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "NovaAct/Widgets/ActEventTimeline/ActActionSequenceWidget.h"
#include "NovaAct/Widgets/ActAssetDetails/ActActionDetailsViewWidget.h"
#include "NovaAct/Controllers/ActAssetDetails/ActAssetDetailsBrain.h"

#include "Animation/AnimBlueprint.h"
#include "Animation/SkeletalMeshActor.h"

#define LOCTEXT_NAMESPACE "NovaAct"

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
			->AddTab(NovaConst::ActActionViewportTabId, ETabState::OpenedTab)
			->SetHideTabWell(true))
		->Split(
			FTabManager::NewStack()
			->AddTab(NovaConst::ActActionSequenceTabId, ETabState::OpenedTab)
			->SetHideTabWell(true))
	);
	// Initialize the asset editor
	InitAssetEditor(EToolkitMode::Standalone, InitToolkitHost, FName("ActAction_AppIdentifier"), StandaloneDefaultLayout, true, true, ActActionSequence);

	// ** 通过对应Widget的Controller，填充Tab的实际内容Widget，并保留对Controller的引用
	// ** ActActionSequenceController
	ActActionSequenceController = MakeShareable(new FActEventTimelineBrain(SharedThis(this)));
	ActActionSequenceController->ExecuteTrackEditorCreateDelegate();
	const ActActionSequence::FActActionSequenceViewParams ViewParams = ActActionSequence::FActActionSequenceViewParams();
	ActActionSequenceController->MakeSequenceWidget(ViewParams);
	ActActionSequenceWidgetParent->SetContent(ActActionSequenceController->GetActActionSequenceWidget());

	// ** PreviewScene(Viewport)Controller
	const FPreviewScene::ConstructionValues ConstructionValues = FPreviewScene::ConstructionValues()
	                                                             .AllowAudioPlayback(true)
	                                                             .ShouldSimulatePhysics(true);
	ActActionPreviewSceneController = MakeShareable(new FActActionPreviewSceneController(ConstructionValues, SharedThis(this)));
	ActActionPreviewSceneController->MakeViewportWidget();
	ActActionViewportWidgetParent->SetContent(ActActionPreviewSceneController->GetActActionViewportWidget());

	// ** DetailsView Controller
	ActActionDetailsViewController = MakeShareable(new FActAssetDetailsBrain(SharedThis(this)));
	ActActionDetailsViewController->MakeDetailsViewWidget();
	if (ActActionDetailsViewWidgetParent)
	{
		ActActionDetailsViewWidgetParent->SetContent(ActActionDetailsViewController->GetActActionDetailsViewWidget());
	}

	// ** Init by resource
	ResetAssetProperties(ActActionSequence);

	// ** 添加属性修改的回调，使其他Controller能接收到Details面板的属性修改事件
	OnAssetPropertiesModified.AddSP(this, &FActActionSequenceEditor::ResetAssetProperties);

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FActActionSequenceEditor::ResetAssetProperties(UObject* InObject)
{
	const UActActionSequence* ActActionSequencePtr = Cast<UActActionSequence>(InObject);
	InitAnimBlueprint(ActActionSequencePtr->AnimBlueprint);
	ActActionSequenceController->AddAnimSequenceTrack(ActActionSequencePtr->AnimSequence);
	if (OnHitBoxesChanged.IsBound())
	{
		OnHitBoxesChanged.Broadcast(ActActionSequencePtr->ActActionHitBoxes);
	}
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
	// ** NOTE:记得反注册中也要添加对应方法
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	FTabSpawnerEntry& TabSpawnerEntryViewport = InTabManager->RegisterTabSpawner(
		NovaConst::ActActionViewportTabId,
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
		NovaConst::ActActionSequenceTabId,
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
		NovaConst::ActActionDetailsViewTabId,
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
	InTabManager->UnregisterTabSpawner(NovaConst::ActActionSequenceTabId);
	InTabManager->UnregisterTabSpawner(NovaConst::ActActionViewportTabId);
	InTabManager->UnregisterTabSpawner(NovaConst::ActActionDetailsViewTabId);

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

void FActActionSequenceEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	UE_LOG(LogActAction, Log, TEXT("PropertyChangedEvent : %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	OnAssetPropertiesModified.Broadcast(ActActionSequence);
}

void FActActionSequenceEditor::InitAnimBlueprint(UAnimBlueprint* AnimBlueprint)
{
	if (!AnimBlueprint || !AnimBlueprint->TargetSkeleton)
	{
		UE_LOG(LogActAction, Log, TEXT("FActActionSequenceEditor::InitAnimBlueprint with nullptr AnimBlueprint or AnimBlueprint->TargetSkeleton is nullptr"));
		return;
	}

	if (ActActionSequence)
	{
		if (ActActionSequence->AnimBlueprint != AnimBlueprint)
		{
			UE_LOG(LogActAction, Log, TEXT("AssignAsEditAnim PreviewActor : %s"), *AnimBlueprint->GetName());
			ActActionSequence->AnimBlueprint = AnimBlueprint;
		}
		ActActionPreviewSceneController->SpawnActorInViewport(ASkeletalMeshActor::StaticClass(), AnimBlueprint);
	}
}

FFrameRate FActActionSequenceEditor::GetTickResolution() const
{
	return ActActionSequence->TickResolution;
}

void FActActionSequenceEditor::SetAnimSequence(UAnimSequence* InAnimSequence) const
{
	ActActionSequence->AnimSequence = InAnimSequence;
}

void FActActionSequenceEditor::AddHitBox() const
{
	if (ActActionSequence)
	{
		ActActionSequence->ActActionHitBoxes.AddDefaulted();
		if (OnHitBoxesChanged.IsBound())
		{
			OnHitBoxesChanged.Broadcast(ActActionSequence->ActActionHitBoxes);
		}
	}
}

#undef LOCTEXT_NAMESPACE
