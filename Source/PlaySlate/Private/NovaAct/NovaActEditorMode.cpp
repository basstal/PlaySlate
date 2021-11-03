#include "NovaActEditorMode.h"

#include "NovaActEditor.h"
#include "Assets/Tabs/TabSpawners.h"
#include "Common/NovaConst.h"

using namespace NovaConst;

FNovaActEditorMode::FNovaActEditorMode(const TSharedRef<FWorkflowCentricApplication>& InWorkflowCentricApplication)
	: FApplicationMode(NovaActEditorMode)
{
	WeakWorkflowCentricApplication = InWorkflowCentricApplication;

	TSharedRef<FActAssetDetailsTabSummoner> ActAssetDetailsTabSummoner = MakeShareable(new FActAssetDetailsTabSummoner(InWorkflowCentricApplication));
	TabFactories.RegisterFactory(ActAssetDetailsTabSummoner);

	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InWorkflowCentricApplication, 0)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InWorkflowCentricApplication, 1)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InWorkflowCentricApplication, 2)));
	TabFactories.RegisterFactory(MakeShareable(new FActViewportSummoner(InWorkflowCentricApplication, 3)));

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
			          ->AddTab(ActAssetDetailsTabId, ETabState::OpenedTab)
			          ->SetSizeCoefficient(0.3f))
		          ->Split(FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
		                                            ->Split(
			                                            FTabManager::NewStack()
			                                            ->AddTab(ActViewportTabIds[0], ETabState::OpenedTab)
			                                            ->SetHideTabWell(true))
		                                            ->Split(
			                                            FTabManager::NewStack()
			                                            ->AddTab(ActEventTimelineTabId, ETabState::OpenedTab)
			                                            ->SetHideTabWell(true))));

	LayoutExtender = MakeShareable(new FLayoutExtender());
	TabLayout->ProcessExtensions(*LayoutExtender);
}

void FNovaActEditorMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager)
{
	TSharedPtr<FWorkflowCentricApplication> WorkflowCentricApplication = WeakWorkflowCentricApplication.Pin();
	WorkflowCentricApplication->RegisterTabSpawners(InTabManager.ToSharedRef());
	WorkflowCentricApplication->PushTabFactories(TabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FNovaActEditorMode::AddTabFactory(FCreateWorkflowTabFactory FactoryCreator)
{
	if (FactoryCreator.IsBound())
	{
		TabFactories.RegisterFactory(FactoryCreator.Execute(WeakWorkflowCentricApplication.Pin()));
	}
}
