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

	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface

protected:
	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

	// The preview scene that we are viewing
	TSharedPtr<FActActionPreviewScene> PreviewScenePtr;
public:
	TSharedPtr<FActActionPreviewScene> GetPreviewScenePtr() const
	{
		return PreviewScenePtr;
	}

protected:
	TSharedPtr<FActActionViewportClient> ViewportClient;
	/**
	 * 入口Editor
	 */
	TWeakPtr<FActActionSequenceEditor> SequenceEditor;
	/** Box that contains notifications */
	TSharedPtr<SVerticalBox> ViewportNotificationsContainer;
};
