#pragma once

#include "NovaAct/ActAssetDetails/ActAssetDetailsWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

struct FActAssetDetailsTabSummoner : public FWorkflowTabFactory
{
public:
	FActAssetDetailsTabSummoner(const TSharedPtr<FAssetEditorToolkit> InHostingApp);

	//~Begin FWorkflowTabFactory interface
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	//~End FWorkflowTabFactory interface

protected:
	TSharedPtr<SActAssetDetailsWidget> DetailsWidget;
};


struct FActViewportSummoner : public FWorkflowTabFactory
{
public:
	FActViewportSummoner(TSharedPtr<FAssetEditorToolkit> InHostingApp, int32 InViewportIndex);

	//~Begin FWorkflowTabFactory interface
	virtual FTabSpawnerEntry& RegisterTabSpawner(TSharedRef<FTabManager> TabManager, const FApplicationMode* CurrentApplicationMode) const override;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	//~End FWorkflowTabFactory interface

protected:
	// TWeakPtr<ISkeletonTree> SkeletonTree;
	// TWeakPtr<IPersonaPreviewScene> PreviewScene;
	// TWeakPtr<FBlueprintEditor> BlueprintEditor;
	// FOnViewportCreated OnViewportCreated;
	// FOnGetViewportText OnGetViewportText;
	// TArray<TSharedPtr<FExtender>> Extenders;
	// FName ContextName;
	// int32 ViewportIndex;
	// bool bShowShowMenu;
	// bool bShowLODMenu;
	// bool bShowPlaySpeedMenu;
	// bool bShowTimeline;
	// bool bShowStats;
	// bool bAlwaysShowTransformToolbar;
	// bool bShowFloorOptions;
	// bool bShowTurnTable;
	// bool bShowPhysicsMenu;
};
