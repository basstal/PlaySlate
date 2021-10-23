#pragma once


class FNovaActEditor;
class FActViewport;
class SActActionViewportWidget;

class FActActionViewportClient : public FEditorViewportClient
{
public:
	FActActionViewportClient(const TSharedRef<FActViewport>& InActActionPreviewSceneController, const TSharedRef<SActActionViewportWidget>& InActActionViewportWidget, FEditorModeTools& InEditorModeTools);
	virtual ~FActActionViewportClient() override;
};
