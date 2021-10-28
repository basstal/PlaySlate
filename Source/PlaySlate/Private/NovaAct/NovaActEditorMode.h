#pragma once

#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

class FWorkflowCentricApplication;

class FNovaActEditorMode : public FApplicationMode
{
public:
	FNovaActEditorMode(const TSharedRef<FWorkflowCentricApplication>& InHostingApp);

	//~Begin FApplicationMode interface
	virtual void RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) override;
	virtual void AddTabFactory(FCreateWorkflowTabFactory FactoryCreator) override;
	//~End FApplicationMode interface
protected:
	/** The hosting app */
	TWeakPtr<FWorkflowCentricApplication> HostingAppPtr;
	/** The tab factories we support */
	FWorkflowAllowedTabSet TabFactories;
};
