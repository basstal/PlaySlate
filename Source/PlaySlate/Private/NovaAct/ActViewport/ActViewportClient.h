#pragma once


class FNovaActEditor;
class FActViewportPreviewScene;
class SActViewport;

class FActViewportClient : public FEditorViewportClient
{
public:
	FActViewportClient(const TSharedRef<FActViewportPreviewScene>& InActViewportPreviewScene,
	                   const TSharedRef<SActViewport>& ActViewport);
	virtual ~FActViewportClient() override;

	//~Begin FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	//~End FEditorViewportClient interface
};
