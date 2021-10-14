#pragma once

#include "SEditorViewport.h"

class FActActionSequenceEditor;
class FActActionViewportClient;
class FActActionPreviewSceneController;

class SActActionViewportWidget : public SEditorViewport, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActActionViewportWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionPreviewSceneController>& InActActionPreviewSceneController);
	virtual ~SActActionViewportWidget() override;
	
	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

protected:
	/**
	 * 这个Widget的Controller
	 */
	TWeakPtr<FActActionPreviewSceneController> ActActionPreviewSceneController;
};


