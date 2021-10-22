#pragma once


class FNovaActEditor;
class FActActionPreviewSceneController;
class SActActionViewportWidget;

class FActActionViewportClient : public FEditorViewportClient
{
public:
	FActActionViewportClient(const TSharedRef<FActActionPreviewSceneController>& InActActionPreviewSceneController, const TSharedRef<SActActionViewportWidget>& InActActionViewportWidget, FEditorModeTools& InEditorModeTools);
	virtual ~FActActionViewportClient() override;
};
