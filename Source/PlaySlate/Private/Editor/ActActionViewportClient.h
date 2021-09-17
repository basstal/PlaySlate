#pragma once
#include "ActActionPreviewScene.h"
#include "SWidget/ActActionViewportWidget.h"

class FActActionViewportClient : public FEditorViewportClient
{
public:
	FActActionViewportClient(const TSharedRef<class FActActionPreviewScene>& InPreviewScene, const TSharedRef<class SActActionViewportWidget>& InViewport, const TSharedRef<class FAssetEditorToolkit>& InAssetEditorToolkit);
	virtual ~FActActionViewportClient() override;

protected:
	TSharedPtr<FActActionPreviewScene> PreviewScenePtr;
	FAssetEditorToolkit* AssetEditorToolkitPtr;
};
