#pragma once


class FActActionSequenceEditor;

class FActActionViewportClient : public FEditorViewportClient
{
public:
	FActActionViewportClient(const TSharedRef<class FActActionPreviewSceneController>& InPreviewScene, const TSharedRef<class SActActionViewportWidget>& InViewport, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~FActActionViewportClient() override;

protected:
	TWeakPtr<FActActionPreviewSceneController> PreviewScenePtr;
	// FAssetEditorToolkit* AssetEditorToolkitPtr;
	// TSharedPtr<SActActionViewportWidget> ViewportWidget;

};
