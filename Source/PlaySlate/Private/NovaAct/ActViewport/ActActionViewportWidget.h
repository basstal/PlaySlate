#pragma once

#include "SEditorViewport.h"

class FNovaActEditor;
class FActActionViewportClient;
class FActViewport;

class SActActionViewportWidget : public SEditorViewport, public FEditorUndoClient
{
public:
SLATE_BEGIN_ARGS(SActActionViewportWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActViewport>& InActActionPreviewSceneController);
	virtual ~SActActionViewportWidget() override;

	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

protected:
	/**
	 * 这个Widget的Controller
	 */
	TWeakPtr<FActViewport> ActActionPreviewSceneController;
};
