#include "ActActionViewportClient.h"

#include "NovaSequenceEditor/Widgets/Viewport/ActActionViewportWidget.h"
#include "ActActionPreviewScene.h"

#include "AssetEditorModeManager.h"
#include "EditorModeManager.h"
#include "UnrealWidget.h"

FActActionViewportClient::FActActionViewportClient(const TSharedRef<FActActionPreviewScene>& InPreviewScene, const TSharedRef<SActActionViewportWidget>& InViewport, const TSharedRef<FAssetEditorToolkit>& InAssetEditorToolkit)
	: FEditorViewportClient(&InAssetEditorToolkit->GetEditorModeManager(), &InPreviewScene.Get(), StaticCastSharedRef<SEditorViewport>(InViewport)),
	  PreviewScenePtr(InPreviewScene),
	  AssetEditorToolkitPtr(&InAssetEditorToolkit.Get())
{
	Widget->SetUsesEditorModeTools(ModeTools.Get());
	FAssetEditorModeManager* ModeManager = (FAssetEditorModeManager*)ModeTools.Get();
	if (ModeManager)
	{
		ModeManager->SetPreviewScene(&InPreviewScene.Get());
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
	PreviewScenePtr.Reset();
	// FEditorViewportClient::~FEditorViewportClient();
}
