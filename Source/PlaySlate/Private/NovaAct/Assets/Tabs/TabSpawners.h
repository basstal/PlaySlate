#pragma once

#include "NovaAct/ActAssetDetails/ActAssetDetailsWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

struct FActAssetDetailsTabSummoner : public FWorkflowTabFactory
{
public:
	FActAssetDetailsTabSummoner(const TSharedRef<FAssetEditorToolkit>& InAssetEditorToolkit);

	//~Begin FWorkflowTabFactory interface
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;
	//~End FWorkflowTabFactory interface

protected:
	TSharedPtr<SActAssetDetailsWidget> ActAssetDetailsWidget;
};


struct FActViewportSummoner : public FWorkflowTabFactory
{
public:
	FActViewportSummoner(const TSharedRef<FAssetEditorToolkit>& InAssetEditorToolkit, int32 InViewportIndex);

	//~Begin FWorkflowTabFactory interface
	virtual FTabSpawnerEntry& RegisterTabSpawner(TSharedRef<FTabManager> TabManager, const FApplicationMode* CurrentApplicationMode) const override;
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	//~End FWorkflowTabFactory interface
};
