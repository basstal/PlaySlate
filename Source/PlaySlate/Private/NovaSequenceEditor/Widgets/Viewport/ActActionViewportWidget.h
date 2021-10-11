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

	void Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InActActionSequenceEditor);
	virtual ~SActActionViewportWidget() override;
	//~Begin SWidget interface
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	//~End SWidget interface
	TSharedPtr<FActActionPreviewSceneController> GetPreviewScenePtr() const;

protected:
	//~Begin SEditorViewport interface
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	//~End SEditorViewport interface

	// The preview scene that we are viewing
	TWeakPtr<FActActionSequenceEditor> ActActionSequenceEditor;
	
	// TSharedPtr<FActActionViewportClient> ActActionViewportClient;
	/** Box that contains notifications */
	TSharedPtr<SVerticalBox> ViewportNotificationsContainer;
	// /**
	//  * 入口Editor
	//  */
	// TWeakPtr<FActActionSequenceEditor> SequenceEditor;
	
};


