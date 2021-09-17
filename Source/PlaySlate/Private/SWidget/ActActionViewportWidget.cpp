#include "ActActionViewportWidget.h"

#include "Editor/ActActionSequenceEditor.h"
#include "Editor/ActActionViewportClient.h"

void SActActionViewportWidget::Construct(const FArguments& InArgs, const TSharedRef<FActActionSequenceEditor>& InSequenceEditor, const TSharedRef<FActActionPreviewScene>& InPreviewScene)
{
	PreviewScenePtr = InPreviewScene;
	SequenceEditor = InSequenceEditor;

	SEditorViewport::Construct(
		SEditorViewport::FArguments()
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
		.AddMetaData<FTagMetaData>(TEXT("ActAction.Viewport"))
	);
}

TSharedRef<FEditorViewportClient> SActActionViewportWidget::MakeEditorViewportClient()
{
	// Create an ActAction viewport client
	ViewportClient = MakeShareable(new FActActionViewportClient(PreviewScenePtr.Pin().ToSharedRef(), SharedThis(this), SequenceEditor.Pin().ToSharedRef()));
	return ViewportClient.ToSharedRef();
}
