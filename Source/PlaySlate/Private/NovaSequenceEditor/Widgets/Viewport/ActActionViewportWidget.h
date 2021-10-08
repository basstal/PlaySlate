#pragma once

#include "SEditorViewport.h"

class FActActionSequenceEditor;
class FActActionViewportClient;
class FActActionPreviewScene;

class SActActionViewportWidget : public SEditorViewport, public FEditorUndoClient
{
public:
	SLATE_BEGIN_ARGS(SActActionViewportWidget)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InSequenceEditor, const TSharedRef<FActActionPreviewScene>& InPreviewScene);
	virtual ~SActActionViewportWidget() override;
	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
public:
	TSharedPtr<FActActionPreviewScene> GetPreviewScenePtr() const
	{
		return PreviewScenePtr;
	}

protected:
	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

	// The preview scene that we are viewing
	TSharedPtr<FActActionPreviewScene> PreviewScenePtr;
	
	TSharedPtr<FActActionViewportClient> ViewportClient;
	/** Box that contains notifications */
	TSharedPtr<SVerticalBox> ViewportNotificationsContainer;
	/**
	 * 入口Editor
	 */
	TWeakPtr<FActActionSequenceEditor> SequenceEditor;
};
