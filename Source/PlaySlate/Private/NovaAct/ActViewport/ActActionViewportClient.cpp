#include "ActActionViewportClient.h"

#include "PlaySlate.h"
#include "NovaAct/Widgets/ActViewport/ActActionViewportWidget.h"
#include "ActViewport.h"

#include "AssetEditorModeManager.h"
#include "EditorModeManager.h"
#include "UnrealWidget.h"

FActActionViewportClient::FActActionViewportClient(const TSharedRef<FActViewport>& InActActionPreviewSceneController, const TSharedRef<SActActionViewportWidget>& InActActionViewportWidget, FEditorModeTools& InEditorModeTools)
	: FEditorViewportClient(&InEditorModeTools, StaticCast<FPreviewScene*>(&InActActionPreviewSceneController.Get()), StaticCastSharedRef<SEditorViewport>(InActActionViewportWidget))
{
	FAssetEditorModeManager* ModeManager = StaticCast<FAssetEditorModeManager*>(ModeTools.Get());
	Widget->SetUsesEditorModeTools(ModeManager);
	if (ModeManager)
	{
		ModeManager->SetPreviewScene(StaticCast<FPreviewScene*>(&InActActionPreviewSceneController.Get()));
	}
	// Default to local space
	ModeTools->SetCoordSystem(COORD_Local);
	Invalidate();
	// DrawHelper set up
	DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;
	DrawHelper.AxesLineThickness = 0.0f;
	// Toggling grid now relies on the show flag
	DrawHelper.bDrawGrid = false;
	ModeTools->SetWidgetMode(UE::Widget::WM_Rotate);
	SetRealtime(true);
	ViewportType = LVT_Perspective;
	bSetListenerPosition = false;
	SetViewLocation(EditorViewportDefs::DefaultPerspectiveViewLocation);
	SetViewRotation(EditorViewportDefs::DefaultPerspectiveViewRotation);
}

FActActionViewportClient::~FActActionViewportClient()
{
	UE_LOG(LogActAction, Log, TEXT("FActActionViewportClient::~FActActionViewportClient"));
}
