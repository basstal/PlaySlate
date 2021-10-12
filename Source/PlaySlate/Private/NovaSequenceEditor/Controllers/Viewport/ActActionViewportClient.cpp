#include "ActActionViewportClient.h"

#include "PlaySlate.h"
#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "ActActionPreviewSceneController.h"

#include "AssetEditorModeManager.h"
#include "EditorModeManager.h"
#include "UnrealWidget.h"
#include "NovaSequenceEditor/ActActionSequenceEditor.h"

FActActionViewportClient::FActActionViewportClient(const TSharedRef<FActActionPreviewSceneController>& InPreviewScene, const TSharedRef<SActActionViewportWidget>& InViewport, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor)
	: FEditorViewportClient(&InActActionSequenceEditor->GetEditorModeManager(), StaticCast<FPreviewScene*>(&InPreviewScene.Get()), StaticCastSharedRef<SEditorViewport>(InViewport)),
	  PreviewScenePtr(InPreviewScene)
// AssetEditorToolkitPtr(InActActionSequenceEditor),
// ViewportWidget(InPreviewScene->GetActActionViewportWidget())
{
	Widget->SetUsesEditorModeTools(ModeTools.Get());
	FAssetEditorModeManager* ModeManager = (FAssetEditorModeManager*)ModeTools.Get();
	if (ModeManager)
	{
		ModeManager->SetPreviewScene(StaticCast<FPreviewScene*>(&InPreviewScene.Get()));
	}
	// ModeTools->SetDefaultMode(ActActionSequence::ActActionViewportEditMode);
	// Default to local space
	ModeTools->SetCoordSystem(COORD_Local);
	Invalidate();
	// DrawHelper set up
	DrawHelper.PerspectiveGridSize = HALF_WORLD_MAX1;
	DrawHelper.AxesLineThickness = 0.0f;
	// Toggling grid now relies on the show flag
	DrawHelper.bDrawGrid = true;
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
	// ViewportWidget.Reset();
	// PreviewScenePtr.Reset();
	// FEditorViewportClient::~FEditorViewportClient();
}
