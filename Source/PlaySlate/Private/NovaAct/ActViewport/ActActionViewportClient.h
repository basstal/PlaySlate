#pragma once


class FNovaActEditor;
class FActViewportPreviewScene;
class SActActionViewportWidget;

class FActActionViewportClient : public FEditorViewportClient
{
public:
	FActActionViewportClient(const TSharedRef<FActViewportPreviewScene>& InActViewportPreviewScene, const TSharedRef<SActActionViewportWidget>& InActActionViewportWidget, FEditorModeTools& InEditorModeTools);
	virtual ~FActActionViewportClient() override;

	//~Begin FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	//~End FEditorViewportClient interface
};
