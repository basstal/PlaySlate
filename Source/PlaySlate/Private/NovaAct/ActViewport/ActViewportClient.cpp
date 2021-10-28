#include "ActViewportClient.h"

#include "PlaySlate.h"
#include "NovaAct/ActViewport/ActViewport.h"
#include "ActViewportPreviewScene.h"

#include "AssetEditorModeManager.h"
#include "EditorModeManager.h"
#include "UnrealWidget.h"

FActViewportClient::FActViewportClient(const TSharedRef<FActViewportPreviewScene>& InActViewportPreviewScene,
                                       const TSharedRef<SActViewport>& ActViewport)
	: FEditorViewportClient(nullptr,
	                        StaticCast<FPreviewScene*>(&InActViewportPreviewScene.Get()),
	                        StaticCastSharedRef<SEditorViewport>(ActViewport))
{
	FAssetEditorModeManager* ModeManager = StaticCast<FAssetEditorModeManager*>(ModeTools.Get());
	Widget->SetUsesEditorModeTools(ModeManager);
	if (ModeManager)
	{
		ModeManager->SetPreviewScene(StaticCast<FPreviewScene*>(&InActViewportPreviewScene.Get()));
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

FActViewportClient::~FActViewportClient()
{
	UE_LOG(LogNovaAct, Log, TEXT("FActViewportClient::~FActViewportClient"));
}

void FActViewportClient::Tick(float DeltaSeconds)
{
	// UE_LOG(LogNovaAct, Log, TEXT("FEditorViewportClient::Tick : %f"), DeltaSeconds);
	FEditorViewportClient::Tick(DeltaSeconds);
}
