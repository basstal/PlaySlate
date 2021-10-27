#pragma once

#include "SEditorViewport.h"

class FNovaActEditor;
class FActViewportClient;
class FActViewportPreviewScene;

class SActViewport : public SEditorViewport, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActViewport) { }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActViewportPreviewScene>& InActActionPreviewSceneController);
	virtual ~SActViewport() override;

	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

protected:
	/**
	 * 这个Widget的Controller
	 */
	TWeakPtr<FActViewportPreviewScene> ActActionPreviewSceneController;
};
