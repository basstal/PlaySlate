#pragma once

#include "SEditorViewport.h"
#include "Editor/ActActionPreviewScene.h"


class FActActionSequenceEditor;
class FActActionViewportClient;

class SActActionViewportWidget : public SEditorViewport, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActActionViewportWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InSequenceEditor, const TSharedRef<FActActionPreviewScene>& InPreviewScene);
protected:
	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

	// The preview scene that we are viewing
	TWeakPtr<FActActionPreviewScene> PreviewScenePtr;
	
	TSharedPtr<FActActionViewportClient> ViewportClient;
	/**
	 * 入口Editor
	 */
	TWeakPtr<FActActionSequenceEditor> SequenceEditor;
};
