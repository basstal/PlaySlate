#include "NovaActEditor.h"

#include "PlaySlate.h"
#include "Common/NovaConst.h"
#include "NovaAct/Assets/ActAnimation.h"
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

FNovaActEditor::FNovaActEditor(UActAnimation* InActAnimation)
	: ActAnimation(InActAnimation),
	  PlaybackRange(0, 0),
	  SelectionRange(0, 0)
{
	check(InActAnimation);
	NovaDB::GetOrCreate<UActAnimation>("ActAnimation", InActAnimation);
}

FNovaActEditor::~FNovaActEditor()
{
	UE_LOG(LogActAction, Log, TEXT("FNovaActEditor::~FNovaActEditor"));
	ActActionPreviewSceneController.Reset();
	ActActionSequenceController.Reset();
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
	InitAssetEditor(EToolkitMode::Standalone, InIToolkitHost, NovaConst::NovaActAppName, StandaloneDefaultLayout, true, true, ActAnimation);

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
	ResetAssetProperties(ActAnimation);

	// ** 添加属性修改的回调，使其他Controller能接收到Details面板的属性修改事件
	OnAssetPropertiesModified.AddSP(this, &FNovaActEditor::ResetAssetProperties);

	// When undo occurs, get a notification so we can make sure our view is up to date
	GEditor->RegisterForUndo(this);
}

void FNovaActEditor::ResetAssetProperties(UObject* InObject)
{
	UActAnimation* ActActionSequencePtr = Cast<UActAnimation>(InObject);
	InitAnimBlueprint(ActActionSequencePtr->AnimBlueprint);
	ActActionSequenceController->AddAnimSequenceTrack(ActActionSequencePtr->AnimSequence);
	if (OnHitBoxesChanged.IsBound())
	{
		OnHitBoxesChanged.Broadcast(ActActionSequencePtr->ActActionHitBoxes);
	}
}

void FNovaActEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(ActAnimation);
}

FString FNovaActEditor::GetReferencerName() const
{
	return "ActActionSequenceEditor";
}

FName FNovaActEditor::GetToolkitFName() const
{
	return FName("ActActionSequenceEditor");
}

FText FNovaActEditor::GetBaseToolkitName() const
{
	return LOCTEXT("AppLabel", "ActAction Sequence Editor");
}

void FNovaActEditor::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	// ** NOTE:记得反注册中也要添加对应方法
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	FTabSpawnerEntry& TabSpawnerEntryViewport = InTabManager->RegisterTabSpawner(
		NovaConst::ActViewportTabId,
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
		NovaConst::ActEventTimelineTabId,
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
		NovaConst::ActAssetDetailsTabId,
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
	return LOCTEXT("WorldCentricTabPrefix", "Sequencer ").ToString();
}

void FNovaActEditor::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged)
{
	UE_LOG(LogActAction, Log, TEXT("PropertyChangedEvent : %s"), *PropertyChangedEvent.GetPropertyName().ToString());
	OnAssetPropertiesModified.Broadcast(ActAnimation);
}

void FNovaActEditor::InitAnimBlueprint(UAnimBlueprint* AnimBlueprint)
{
	if (!AnimBlueprint || !AnimBlueprint->TargetSkeleton)
	{
		UE_LOG(LogActAction, Log, TEXT("FNovaActEditor::InitAnimBlueprint with nullptr AnimBlueprint or AnimBlueprint->TargetSkeleton is nullptr"));
		return;
	}

	if (ActAnimation)
	{
		if (ActAnimation->AnimBlueprint != AnimBlueprint)
		{
			UE_LOG(LogActAction, Log, TEXT("AssignAsEditAnim PreviewActor : %s"), *AnimBlueprint->GetName());
			ActAnimation->AnimBlueprint = AnimBlueprint;
		}
		ActActionPreviewSceneController->SpawnActorInViewport(ASkeletalMeshActor::StaticClass(), AnimBlueprint);
	}
}

FFrameRate FNovaActEditor::GetTickResolution() const
{
	return ActAnimation->TickResolution;
}

void FNovaActEditor::SetAnimSequence(UAnimSequence* InAnimSequence) const
{
	ActAnimation->AnimSequence = InAnimSequence;
}

void FNovaActEditor::AddHitBox() const
{
	if (ActAnimation)
	{
		ActAnimation->ActActionHitBoxes.AddDefaulted();
		if (OnHitBoxesChanged.IsBound())
		{
			OnHitBoxesChanged.Broadcast(ActAnimation->ActActionHitBoxes);
		}
	}
}

#undef LOCTEXT_NAMESPACE
