#include "NovaActEditorMode.h"

#include "NovaActEditor.h"
#include "Assets/Tabs/TabSpawners.h"
#include "Common/NovaConst.h"

FNovaActEditorMode::FNovaActEditorMode(const TSharedRef<FWorkflowCentricApplication>& InHostingApp)
	: FApplicationMode(NovaConst::NovaActEditorMode)
{
	HostingAppPtr = InHostingApp;

	TSharedRef<FNovaActEditor> AnimationEditor = StaticCastSharedRef<FNovaActEditor>(InHostingApp);

	// ISkeletonEditorModule& SkeletonEditorModule = FModuleManager::LoadModuleChecked<ISkeletonEditorModule>("SkeletonEditor");
	// TabFactories.RegisterFactory(SkeletonEditorModule.CreateSkeletonTreeTabFactory(InHostingApp, InSkeletonTree));

	// FOnObjectsSelected OnObjectsSelected = FOnObjectsSelected::CreateSP(&AnimationEditor.Get(), &FNovaActEditor::HandleObjectsSelected);

	// FPersonaModule& PersonaModule = FModuleManager::LoadModuleChecked<FPersonaModule>("Persona");
	TSharedRef<FActAssetDetailsTabSummoner> ActAssetDetailsTabSummoner = MakeShareable(new FActAssetDetailsTabSummoner(InHostingApp));
	TabFactories.RegisterFactory(ActAssetDetailsTabSummoner);

	// FPersonaViewportArgs ViewportArgs(AnimationEditor->GetPersonaToolkit()->GetPreviewScene());
	// ViewportArgs.bShowTimeline = false;
	// ViewportArgs.ContextName = TEXT("AnimationEditor.Viewport");

	// PersonaModule.RegisterPersonaViewportTabFactories(TabFactories, InHostingApp, ViewportArgs);
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InHostingApp, 0)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InHostingApp, 1)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InHostingApp, 2)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InHostingApp, 3)));

	// TabFactories.RegisterFactory(PersonaModule.CreateAdvancedPreviewSceneTabFactory(InHostingApp, AnimationEditor->GetPersonaToolkit()->GetPreviewScene()));
	// TabFactories.RegisterFactory(PersonaModule.CreateAnimationAssetBrowserTabFactory(InHostingApp, AnimationEditor->GetPersonaToolkit(), FOnOpenNewAsset::CreateSP(&AnimationEditor.Get(), &FNovaActEditor::HandleOpenNewAsset), FOnAnimationSequenceBrowserCreated::CreateSP(&AnimationEditor.Get(), &FNovaActEditor::HandleAnimationSequenceBrowserCreated), true));
	// TabFactories.RegisterFactory(PersonaModule.CreateAssetDetailsTabFactory(InHostingApp, FOnGetAsset::CreateSP(&AnimationEditor.Get(), &FNovaActEditor::HandleGetAsset), FOnDetailsCreated()));
	// TabFactories.RegisterFactory(PersonaModule.CreateCurveViewerTabFactory(InHostingApp, InSkeletonTree->GetEditableSkeleton(), AnimationEditor->GetPersonaToolkit()->GetPreviewScene(), OnObjectsSelected));
	// TabFactories.RegisterFactory(PersonaModule.CreateSkeletonSlotNamesTabFactory(InHostingApp, InSkeletonTree->GetEditableSkeleton(), FOnObjectSelected::CreateSP(&AnimationEditor.Get(), &FNovaActEditor::HandleObjectSelected)));
	// TabFactories.RegisterFactory(PersonaModule.CreateAnimNotifiesTabFactory(InHostingApp, InSkeletonTree->GetEditableSkeleton(), OnObjectsSelected));
	// TabFactories.RegisterFactory(PersonaModule.CreateAnimMontageSectionsTabFactory(InHostingApp, AnimationEditor->GetPersonaToolkit(), AnimationEditor->OnSectionsChanged));

	/**
	 * Editor（大页签）内的每个窗口由一个个Tab组成，这里载入这些Tab和对应的Layout信息，
	 * 注意：AddTab必须是已经注册过的TabId，注册的过程是通过重载RegisterTabSpawners
	 * 这个默认的Layout只在恢复默认Layout时有效，其他时候都是读取Layout在ini中的缓存信息，即加载之前保存的Layout
	 */
	TabLayout = FTabManager::NewLayout("Standalone_NovaActEditor")
		->AddArea(FTabManager::NewPrimaryArea()
		          ->SetOrientation(Orient_Horizontal)
		          ->Split(
			          FTabManager::NewStack()
			          ->AddTab(NovaConst::ActAssetDetailsTabId, ETabState::OpenedTab)
			          ->SetSizeCoefficient(0.3f))
		          ->Split(FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
		                                            ->Split(
			                                            FTabManager::NewStack()
			                                            ->AddTab(NovaConst::ActViewportTabIds[0], ETabState::OpenedTab)
			                                            ->SetHideTabWell(true))
		                                            ->Split(
			                                            FTabManager::NewStack()
			                                            ->AddTab(NovaConst::ActEventTimelineTabId, ETabState::OpenedTab)
			                                            ->SetHideTabWell(true))));

	// TabLayout = FTabManager::NewLayout("Standalone_AnimationEditor_Layout_v1.4")
	// 	->AddArea(
	// 		FTabManager::NewPrimaryArea()
	// 		->SetOrientation(Orient_Vertical)
	// 		->Split(
	// 			FTabManager::NewSplitter()
	// 			->SetSizeCoefficient(0.9f)
	// 			->SetOrientation(Orient_Horizontal)
	// 			->Split(
	// 				FTabManager::NewSplitter()
	// 				->SetSizeCoefficient(0.2f)
	// 				->SetOrientation(Orient_Vertical)
	// 				->Split(
	// 					FTabManager::NewStack()
	// 					->SetHideTabWell(false)
	// 					->AddTab(AnimationEditorTabs::SkeletonTreeTab, ETabState::OpenedTab)
	// 					->AddTab(AnimationEditorTabs::AssetDetailsTab, ETabState::OpenedTab)
	// 				)
	// 			)
	// 			->Split(
	// 				FTabManager::NewSplitter()
	// 				->SetSizeCoefficient(0.6f)
	// 				->SetOrientation(Orient_Vertical)
	// 				->Split(
	// 					FTabManager::NewStack()
	// 					->SetSizeCoefficient(0.6f)
	// 					->SetHideTabWell(true)
	// 					->AddTab(AnimationEditorTabs::ViewportTab, ETabState::OpenedTab)
	// 				)
	// 				->Split(
	// 					FTabManager::NewStack()
	// 					->SetSizeCoefficient(0.4f)
	// 					->SetHideTabWell(true)
	// 					->AddTab(AnimationEditorTabs::DocumentTab, ETabState::ClosedTab)
	// 					->AddTab(AnimationEditorTabs::CurveEditorTab, ETabState::ClosedTab)
	// 					->SetForegroundTab(AnimationEditorTabs::DocumentTab)
	// 				)
	// 			)
	// 			->Split(
	// 				FTabManager::NewSplitter()
	// 				->SetSizeCoefficient(0.2f)
	// 				->SetOrientation(Orient_Vertical)
	// 				->Split(
	// 					FTabManager::NewStack()
	// 					->SetSizeCoefficient(0.6f)
	// 					->SetHideTabWell(false)
	// 					->AddTab(AnimationEditorTabs::DetailsTab, ETabState::OpenedTab)
	// 					->AddTab(AnimationEditorTabs::AdvancedPreviewTab, ETabState::OpenedTab)
	// 					->SetForegroundTab(AnimationEditorTabs::DetailsTab)
	// 				)
	// 				->Split(
	// 					FTabManager::NewStack()
	// 					->SetSizeCoefficient(0.4f)
	// 					->SetHideTabWell(false)
	// 					->AddTab(AnimationEditorTabs::AssetBrowserTab, ETabState::OpenedTab)
	// 					->AddTab(AnimationEditorTabs::AnimMontageSectionsTab, ETabState::ClosedTab)
	// 					->AddTab(AnimationEditorTabs::CurveNamesTab, ETabState::ClosedTab)
	// 					->AddTab(AnimationEditorTabs::SlotNamesTab, ETabState::ClosedTab)
	// 				)
	// 			)
	// 		)
	// 	);

	// PersonaModule.OnRegisterTabs().Broadcast(TabFactories, InHostingApp);
	LayoutExtender = MakeShared<FLayoutExtender>();
	// PersonaModule.OnRegisterLayoutExtensions().Broadcast(*LayoutExtender.Get());
	TabLayout->ProcessExtensions(*LayoutExtender.Get());
}

void FNovaActEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FWorkflowCentricApplication> HostingApp = HostingAppPtr.Pin();
	HostingApp->RegisterTabSpawners(InTabManager.ToSharedRef());
	HostingApp->PushTabFactories(TabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FNovaActEditorMode::AddTabFactory(FCreateWorkflowTabFactory FactoryCreator)
{
	if (FactoryCreator.IsBound())
	{
		TabFactories.RegisterFactory(FactoryCreator.Execute(HostingAppPtr.Pin()));
	}
}
