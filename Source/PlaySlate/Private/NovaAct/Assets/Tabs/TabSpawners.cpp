#include "TabSpawners.h"

#include "Common/NovaConst.h"
#include "NovaAct/ActViewport/ActViewportTabWidget.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

#define LOCTEXT_NAMESPACE "NovaAct"

FActAssetDetailsTabSummoner::FActAssetDetailsTabSummoner(const TSharedPtr<FAssetEditorToolkit> InHostingApp)
	: FWorkflowTabFactory(NovaConst::ActAssetDetailsTabId, InHostingApp)
{
	TabLabel = LOCTEXT("ActAssetDetails", "NovaActEditor ActAssetDetails tab name.");
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Details");
	bIsSingleton = true;

	ViewMenuDescription = LOCTEXT("DetailsDescription", "Details");
	ViewMenuTooltip = LOCTEXT("DetailsToolTip", "Shows the details tab for selected objects.");

	DetailsWidget = SNew(SActAssetDetailsWidget);
}

TSharedRef<SWidget> FActAssetDetailsTabSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
	return DetailsWidget.ToSharedRef();
}

FText FActAssetDetailsTabSummoner::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return LOCTEXT("PersonaDetailsToolTip", "Edit the details of selected objects.");
}

FActViewportSummoner::FActViewportSummoner(TSharedPtr<FAssetEditorToolkit> InHostingApp, int32 InViewportIndex)
	: FWorkflowTabFactory(NovaConst::ActViewportTabIds[InViewportIndex], InHostingApp)
{
	TabLabel = FText::Format(LOCTEXT("ViewportTabTitle", "Viewport {0}"), FText::AsNumber(InViewportIndex + 1));
	TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports");

	bIsSingleton = true;

	ViewMenuDescription = FText::Format(LOCTEXT("ViewportViewFormat", "Viewport {0}"), FText::AsNumber(InViewportIndex + 1));
	ViewMenuTooltip = LOCTEXT("ActViewport", "NovaActEditor ActViewport tab name.");
}

FTabSpawnerEntry& FActViewportSummoner::RegisterTabSpawner(TSharedRef<FTabManager> TabManager, const FApplicationMode* CurrentApplicationMode) const
{
	FTabSpawnerEntry& SpawnerEntry = FWorkflowTabFactory::RegisterTabSpawner(TabManager, nullptr);

	if (CurrentApplicationMode)
	{
		// find an existing workspace item or create new
		TSharedPtr<FWorkspaceItem> GroupItem = nullptr;
		TSharedRef<FWorkspaceItem> MenuCategory = CurrentApplicationMode->GetWorkspaceMenuCategory();
		for (const TSharedRef<FWorkspaceItem>& Item : MenuCategory->GetChildItems())
		{
			if (Item->GetDisplayName().ToString() == LOCTEXT("ViewportsSubMenu", "Viewports").ToString())
			{
				GroupItem = Item;
				break;
			}
		}

		if (!GroupItem.IsValid())
		{
			GroupItem = MenuCategory->AddGroup(LOCTEXT("ViewportsSubMenu", "Viewports"),
			                                   LOCTEXT("ViewportsSubMenu_Tooltip", "Open a new viewport on the scene"),
			                                   FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.Tabs.Viewports"));
		}

		SpawnerEntry.SetGroup(GroupItem.ToSharedRef());
	}

	return SpawnerEntry;
}

TSharedRef<SWidget> FActViewportSummoner::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
		// .BlueprintEditor(BlueprintEditor.Pin())
		// .OnInvokeTab(FOnInvokeTab::CreateSP(HostingApp.Pin().Get(), &FAssetEditorToolkit::InvokeTab))
		// .AddMetaData<FTagMetaData>(TEXT("Persona.Viewport"))
		// .Extenders(Extenders)
		// .ContextName(ContextName)
		// .OnGetViewportText(OnGetViewportText)
		// .ShowShowMenu(bShowShowMenu)
		// .ShowLODMenu(bShowLODMenu)
		// .ShowPlaySpeedMenu(bShowPlaySpeedMenu)
		// .ShowTimeline(bShowTimeline)
		// .ShowStats(bShowStats)
		// .AlwaysShowTransformToolbar(bAlwaysShowTransformToolbar)
		// .ShowFloorOptions(bShowFloorOptions)
		// .ShowTurnTable(bShowTurnTable)
		// .ShowPhysicsMenu(bShowPhysicsMenu);

	// OnViewportCreated.ExecuteIfBound(NewViewport);

	return SNew(SActViewportTabWidget);
}


#undef LOCTEXT_NAMESPACE
